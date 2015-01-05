#include "../SelfLoader/SelfLoader.h"

int main(void){
	void* pebPtr = slGetPEB();
	void* wow32Reserved = slGetWow32Reserved();

	PPEB32 peb = (PPEB32)pebPtr;
	PLIST_ENTRY moduleListTail = &peb->Ldr->InMemoryOrderModuleList;
	PLIST_ENTRY moduleList = moduleListTail->Flink;

	wprintf(L"Default loaded modules:\n");
	do {
		char* modulePtrWithOffset = (char*)moduleList;
		PLDR_DATA_TABLE_ENTRY module = (PLDR_DATA_TABLE_ENTRY)modulePtrWithOffset;
		wprintf(L"Module full name: %s\n", module->FullDllName.Buffer);
		wprintf(L"Module DLL Base: %p\n", module->Reserved2[0]);
		wprintf(L"Module Entrypoint: %p\n", module->Reserved2[1]);

		void* DllBase = module->Reserved2[0];
		PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)DllBase; //module->DllBase;
		if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE){
			wprintf(L"\n");
			moduleList = moduleList->Flink;
			continue;
		}
		wprintf(L"Module has a legal DOS header.\n");
		PIMAGE_NT_HEADERS32 headers32 = (PIMAGE_NT_HEADERS32)((char*)DllBase + dosHeader->e_lfanew);
		if (headers32->Signature != IMAGE_NT_SIGNATURE){
			wprintf(L"\n");
			moduleList = moduleList->Flink;
			continue;
		}
		wprintf(L"Module has a legal PE32 header.\n");
		if (headers32->FileHeader.SizeOfOptionalHeader < 96 || headers32->OptionalHeader.NumberOfRvaAndSizes == 0){
			wprintf(L"\n");
			moduleList = moduleList->Flink;
			continue;
		}
		wprintf(L"Module has a legal optional header.\n");
		DWORD EdtOffset = headers32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		if (!EdtOffset){
			wprintf(L"\n");
			moduleList = moduleList->Flink;
			continue;
		}
		PEXPORT_DIRECTORY_TABLE EdtPtr = (PEXPORT_DIRECTORY_TABLE)((char*)DllBase + EdtOffset);
		PVOID OrdinalTable = (PBYTE)DllBase + EdtPtr->OrdinalTableRVA;
		PVOID NamePointerTable = (PBYTE)DllBase + EdtPtr->NamePointerTableRVA;
		PVOID ExportAddressTable = (PBYTE)DllBase + EdtPtr->ExportAddressTableRVA;

		wprintf(L"Exported symbols:\n");
		for (DWORD i = 0; i < EdtPtr->NamePointerTableSize; i++){
			WORD Ordinal = ((PWORD)OrdinalTable)[i] + (WORD)EdtPtr->OrdinalBase;
			DWORD NameRVA = ((PDWORD)NamePointerTable)[i];
			WORD RealOrdinal = Ordinal - (WORD)EdtPtr->OrdinalBase;
			DWORD ExportAddress = 0;
			if (Ordinal > 0)
				ExportAddress = ((PDWORD)ExportAddressTable)[RealOrdinal];


			char* NameAddr = (char*)DllBase + NameRVA;
			void* SymbolAddr = (void*)ExportAddress; //(char*)DllBase + ExportAddress;

			printf("\t\tSymbol Name: %s\n", NameAddr);
			wprintf(L"\t\tOrdinal: %hu\n", Ordinal);
			wprintf(L"\t\tExported Virtual Address: %p\n", SymbolAddr);
		}
		moduleList = moduleList->Flink;
	} while (moduleList != moduleListTail);

	return 0;
}
