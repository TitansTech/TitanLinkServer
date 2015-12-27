
#if (MEM_PROTECT==1)

extern int removeFunctionsArray[255];
extern int imageBaseOfGS;
extern int replacementRange;
extern BYTE executedParentSearch;
extern BYTE replacementPart;
extern BYTE jmpLength;
ULONG_PTR GetParentProcessId();
void ReplaceBytes(DWORD Address, BYTE ReplaceTo, int HowMany);
void DestroyFunction(DWORD Address, BYTE ReplaceTo);


inline DWORD GetExplorerPIDbyShellWindow()
{
	DWORD PID = 0;
	
	// Get the PID 
	GetWindowThreadProcessId(GetShellWindow(), &PID);

	return PID;
}

#endif