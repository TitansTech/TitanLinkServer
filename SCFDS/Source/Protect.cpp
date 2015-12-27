#include "stdafx.h"
#include "..\stdafx.h"

#if (MEM_PROTECT==1)

int removeFunctionsArray[255];
int imageBaseOfGS = 0x00400000;
int replacementRange = 0x00400000;
BYTE executedParentSearch = 0;
BYTE replacementPart = 0xCC;
BYTE jmpLength = 5;

ULONG_PTR GetParentProcessId()
{
#if (WL_PROTECT==1)
	VM_START
#endif

	ULONG_PTR pbi[6];
	ULONG ulSize = 0;  
	
	LONG (WINAPI *NtQueryInformationProcess)(HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
	*(FARPROC *)&NtQueryInformationProcess = GetProcAddress(LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");
	
	if(NtQueryInformationProcess){
		if(NtQueryInformationProcess(GetCurrentProcess(), 0, &pbi, sizeof(pbi), &ulSize) >= 0 && ulSize == sizeof(pbi))
		{
			executedParentSearch = 1;
			return pbi[5];
		}
	}
	return (ULONG_PTR)-1; 

#if (WL_PROTECT==1)
	VM_END
#endif
} 

void ReplaceBytes(DWORD Address, BYTE ReplaceTo, int HowMany)
{
#if (WL_PROTECT==1)
	VM_START
#endif
		
	HANDLE hProc = GetCurrentProcess();

	if (HowMany < 0)
	{
		HowMany = 25;
	}

	for(int i=0;i<HowMany;i++)
	{
		WriteProcessMemory(hProc, (LPVOID)(Address+i), &ReplaceTo, 1, NULL);	
		//*(BYTE*)(Address+i) = ReplaceTo;
	}

#if (WL_PROTECT==1)
	VM_END
#endif
}

void DestroyFunction(DWORD Address, BYTE ReplaceTo)
{
#if (WL_PROTECT==1)
	VM_START
#endif

	BYTE newvalue[] = {0x0};
	HANDLE hProc = GetCurrentProcess();
	int i = 0;
	int counter = 0;
	newvalue[0] = ReplaceTo;

	while (counter < 6)
	{
		BYTE tempVal = 0;
		ReadProcessMemory(hProc, (LPVOID)(Address+i), (LPVOID)&tempVal, 1, NULL);

		//*(BYTE*)(Address+i) = ReplaceTo;
		//if (*(BYTE*)(Address+i)==0xCC)
		if (tempVal==0xCC)
		{
			counter++;
		} else {
			counter = 0;
		}
		WriteProcessMemory(hProc, (LPVOID)(Address+i), (LPVOID)&newvalue, 1, NULL);

		if ((*(BYTE*)(Address+i)==0xCC && *(BYTE*)(Address+i+1)==0x55) ||									//INT 3 PUSH EBP
			(*(BYTE*)(Address+i)==0xC3 && *(BYTE*)(Address+i+1)==0xCC) ||									//RETN INT 3
			(*(BYTE*)(Address+i)==0x5D && *(BYTE*)(Address+i+1)==0xC3) ||									//RETN INT 3
			(*(BYTE*)(Address+i)==0x5D && *(BYTE*)(Address+i+1)==0xC2 && *(BYTE*)(Address+i+3)==0x00) ||	//RETN *
			(*(BYTE*)(Address+i+1)==0x55 && *(BYTE*)(Address+i+2)==0x8B && *(BYTE*)(Address+i+3)==0xEC &&	//PUSH EBP
			 *(BYTE*)(Address+i+4)==0x83 && *(BYTE*)(Address+i+5)==0xEC) ||									//MOV EBP,ESP
			(*(BYTE*)(Address+i)==0x33 && *(BYTE*)(Address+i+1)==0xC0 && *(BYTE*)(Address+i+2)==0xC9 &&		//LEAVE
			 (*(BYTE*)(Address+i+3)==0xC3 || (*(BYTE*)(Address+i+3)==0xC2 && *(BYTE*)(Address+i+5)==0x00)))	//RETN X
			 )
		{
			return;
		}

		i++;
	}

#if (WL_PROTECT==1)
	VM_END
#endif
}

#endif