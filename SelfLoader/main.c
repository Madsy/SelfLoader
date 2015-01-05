#include "SelfLoader.h"

static int slStrCompare(LPCSTR s1, LPCSTR s2){
	while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2){
		s1++;
		s2++;
	}
	int i = 0;
	if (*s1 < *s2) i = -1;
	else if (*s1 > *s2) i = 1;
	else i = 0;
	return i;
}

static int slWStrCompare(LPCWSTR s1, LPCWSTR s2){	
	while (*s1 != L'\0' && *s2 != L'\0' && *s1 == *s2){
		s1++;
		s2++;
	}
	int i = 0;
	if (*s1 < *s2) i = -1;
	else if (*s1 > *s2) i = 1;
	else i = 0;	
	return i;
}

void* slGetModuleBase(LPCWSTR moduleName){
	void* pebPtr = slGetPEB();	
	PPEB32 peb = (PPEB32)pebPtr;
	PLIST_ENTRY moduleListTail = &peb->Ldr->InMemoryOrderModuleList;
	PLIST_ENTRY moduleList = moduleListTail->Flink;
	do {
		PLDR_DATA_TABLE_ENTRY module = (PLDR_DATA_TABLE_ENTRY)moduleList;
		if (!slWStrCompare(module->FullDllName.Buffer, moduleName)){
			void* DllBase = module->Reserved2[0];
			return DllBase;
		}
		moduleList = moduleList->Flink;
	} while (moduleList != moduleListTail);
	return NULL;
}

void* slGetModuleProcAddress(void* moduleBase, LPCSTR procName){
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)moduleBase;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;	
	PIMAGE_NT_HEADERS32 headers32 = (PIMAGE_NT_HEADERS32)((char*)moduleBase + dosHeader->e_lfanew);
	if (headers32->Signature != IMAGE_NT_SIGNATURE)
		return NULL;	
	if (headers32->FileHeader.SizeOfOptionalHeader < 96 || headers32->OptionalHeader.NumberOfRvaAndSizes == 0)
		return NULL;
	
	DWORD EdtOffset = headers32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	if (!EdtOffset)
		return NULL;

	PEXPORT_DIRECTORY_TABLE EdtPtr = (PEXPORT_DIRECTORY_TABLE)((char*)moduleBase + EdtOffset);
	PVOID OrdinalTable = (PBYTE)moduleBase + EdtPtr->OrdinalTableRVA;
	PVOID NamePointerTable = (PBYTE)moduleBase + EdtPtr->NamePointerTableRVA;
	PVOID ExportAddressTable = (PBYTE)moduleBase + EdtPtr->ExportAddressTableRVA;
	
	for (DWORD i = 0; i < EdtPtr->NamePointerTableSize; i++){		
		DWORD NameRVA = ((PDWORD)NamePointerTable)[i];
		const char* NameAddr = (char*)moduleBase + NameRVA;

		if (slStrCompare(NameAddr, procName))
			continue;

		WORD Ordinal = ((PWORD)OrdinalTable)[i] + (WORD)EdtPtr->OrdinalBase;
		WORD RealOrdinal = Ordinal - (WORD)EdtPtr->OrdinalBase;
		DWORD ExportAddress = 0;		
		ExportAddress = ((PDWORD)ExportAddressTable)[RealOrdinal];		
		void* FinalAddr = (char*)moduleBase + ExportAddress;
		return FinalAddr;
	}
	return NULL;
}

//kernel32
typedef HMODULE(WINAPI *LOADLIBRARYWPROC)(LPCWSTR);
typedef BOOL(WINAPI *FREELIBRARYPROC)(HMODULE);
typedef PVOID(WINAPI *GETPROCADDRESSSPROC)(HMODULE, LPCSTR);
typedef VOID(WINAPI *EXITPROCESSPROC)(UINT);
//user32
typedef int (WINAPI *MESSAGEBOXWPROC)(HWND, LPCWSTR, LPCWSTR, UINT);

#ifdef _DEBUG
int main(void){
	//kernel32
	EXITPROCESSPROC slExitProcess = NULL;
	LOADLIBRARYWPROC slLoadLibraryW = NULL;
	FREELIBRARYPROC slFreeLibrary = NULL;
	GETPROCADDRESSSPROC slGetProcAddress = NULL;
	//user32
	MESSAGEBOXWPROC slMessageBoxW = NULL;
	HMODULE m;

	void* kernel32 = slGetModuleBase(L"KERNEL32.DLL");
	if (!kernel32) return 0;

	if ((slExitProcess = slGetModuleProcAddress(kernel32, "ExitProcess")) == NULL) return 0;
	if ((slLoadLibraryW = slGetModuleProcAddress(kernel32, "LoadLibraryW")) == NULL) return 0;
	if ((slFreeLibrary = slGetModuleProcAddress(kernel32, "FreeLibrary")) == NULL) return 0;
	if ((slGetProcAddress = slGetModuleProcAddress(kernel32, "GetProcAddress")) == NULL) return 0;

	if ((m = slLoadLibraryW(L"USER32.DLL")) == NULL) return 0;
	if ((slMessageBoxW = slGetProcAddress(m, "MessageBoxW")) == NULL) return 0;

	slMessageBoxW(0, L"SelfLoader Example", L"Test", MB_OK);
	slFreeLibrary(m);	

	return 0;
}
#else
/* Release. Less error checking */
void main(void){
	//kernel32
	EXITPROCESSPROC slExitProcess = NULL;
	LOADLIBRARYWPROC slLoadLibraryW = NULL;
	FREELIBRARYPROC slFreeLibrary = NULL;
	GETPROCADDRESSSPROC slGetProcAddress = NULL;
	//user32
	MESSAGEBOXWPROC slMessageBoxW = NULL;
	HMODULE m;

	void* kernel32 = slGetModuleBase(L"KERNEL32.DLL");

	slExitProcess = slGetModuleProcAddress(kernel32, "ExitProcess");
	slLoadLibraryW = slGetModuleProcAddress(kernel32, "LoadLibraryW");
	slFreeLibrary = slGetModuleProcAddress(kernel32, "FreeLibrary");
	slGetProcAddress = slGetModuleProcAddress(kernel32, "GetProcAddress");

	m = slLoadLibraryW(L"USER32.DLL");
	slMessageBoxW = slGetProcAddress(m, "MessageBoxW");

	slMessageBoxW(0, L"SelfLoader Example", L"Test", MB_OK);
	slFreeLibrary(m);

	slExitProcess(0);
}
#endif

