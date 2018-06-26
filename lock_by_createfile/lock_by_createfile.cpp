// lock_by_createfile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

typedef struct _SYSTEM_HANDLE_STATE
{
	DWORD r1;
	DWORD GrantedAccess;
	DWORD HandleCount;
	DWORD ReferenceCount;
	DWORD r5;
	DWORD r6;
	DWORD r7;
	DWORD r8;
	DWORD r9;
	DWORD r10;
	DWORD r11;
	DWORD r12;
	DWORD r13;
	DWORD r14;
} SYSTEM_HANDLE_STATE, *PSYSTEM_HANDLE_STATE;

typedef long (__stdcall *NTQUERYOBJECT)(
	HANDLE ObjectHandle,
	ULONG ObjectInformationClass,
	PVOID ObjectInformation,
	ULONG ObjectInformationLength,
	PULONG ReturnLength
	);

int check_handle_count(HANDLE handle, int &count)
{
	NTQUERYOBJECT pfnNtQueryObject =
		reinterpret_cast<NTQUERYOBJECT>(GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryObject"));
	if (NULL == pfnNtQueryObject)
	{
		return -1;
	}

	SYSTEM_HANDLE_STATE shs = {0};
	ULONG len = 0;

	long ret = pfnNtQueryObject(handle, 0, &shs, sizeof(shs), &len);
	if (0xC0000008 == ret)
	{
		count = 0;
		return -2;
	}

	count = shs.HandleCount; // Subtract 1 for handle count
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hfile = CreateFile(
		L"D:\\test.mp4",
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hfile)
	{
		printf("CreateFile failed: %d\n", GetLastError());
		return -1;
	}

	int count = 0;
	check_handle_count(hfile, count);
	printf("count: %d\n", count);

	printf("CreateFile succeed, handle: %d, guess you cannot move that file!\n", hfile);
	getchar();

	CloseHandle(hfile);

	check_handle_count(hfile, count);
	printf("count: %d\n", count);

	return 0;
}

