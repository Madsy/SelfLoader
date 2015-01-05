#ifndef SELF_LOADER_H_GUARD
#define SELF_LOADER_H_GUARD

#include <Windows.h>
#include <Subauth.h>
#include <stdio.h>

typedef struct _EXPORT_DIRECTORY_TABLE {
	DWORD	ExportFlags;
	DWORD	TimeStamp;
	WORD	MajorVersion;
	WORD	MinorVersion;
	DWORD	NameRVA;
	DWORD	OrdinalBase;
	DWORD	ExportAddressTableSize;
	DWORD	NamePointerTableSize;
	DWORD	ExportAddressTableRVA;
	DWORD	NamePointerTableRVA;
	DWORD	OrdinalTableRVA;
} EXPORT_DIRECTORY_TABLE, *PEXPORT_DIRECTORY_TABLE;

typedef struct _PEB_LDR_DATA {
	BYTE       Reserved1[8];
	PVOID      Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

/* Already defined in wint.h (1049) */
/*
typedef struct _LIST_ENTRY {
struct _LIST_ENTRY *Flink;
struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;
*/

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE           Reserved1[16];
	PVOID          Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _LDR_DATA_TABLE_ENTRY {
	PVOID Reserved1[2];
	LIST_ENTRY InMemoryOrderLinks;
	PVOID Reserved2[2];
	PVOID DllBase;
	PVOID EntryPoint;
	PVOID Reserved3;
	UNICODE_STRING FullDllName;
	BYTE Reserved4[8];
	PVOID Reserved5[3];
	union {
		ULONG CheckSum;
		PVOID Reserved6;
	};
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef void(__stdcall * PPS_POST_PROCESS_INIT_ROUTINE)(void);

typedef struct _PEB32 {
	BYTE                          Reserved1[2];
	BYTE                          BeingDebugged;
	BYTE                          Reserved2[1];
	PVOID                         Reserved3[2];
	PPEB_LDR_DATA                 Ldr;
	PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
	BYTE                          Reserved4[104];
	PVOID                         Reserved5[52];
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE                          Reserved6[128];
	PVOID                         Reserved7[1];
	ULONG                         SessionId;
} PEB32, *PPEB32;

typedef struct _PEB64 {
	BYTE							Reserved1[2];
	BYTE							BeingDebugged;
	BYTE							Reserved2[21];
	PPEB_LDR_DATA					LoaderData;
	PRTL_USER_PROCESS_PARAMETERS	ProcessParameters;
	BYTE							Reserved3[520];
	PPS_POST_PROCESS_INIT_ROUTINE	PostProcessInitRoutine;
	BYTE							Reserved4[136];
	ULONG							SessionId;
} PEB64, *PPEB64;

void* slGetPEB(void);
void* slGetWow32Reserved(void);

#endif