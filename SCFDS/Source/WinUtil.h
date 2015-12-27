// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
//------------------------------------------
// WinUtil.h	: Status Exactly as GameServer
//------------------------------------------
#ifndef __WINUTIL_H
#define __WINUTIL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//------------------------------------------
// WinUtil.cpp Functions
//------------------------------------------
void BuxConvert(char* buf, int size);
void PHeadSetB(LPBYTE lpBuf, BYTE head, int size);
void PHeadSubSetB(LPBYTE lpBuf, BYTE head, BYTE sub, int size);
void PHeadSetW( LPBYTE lpBuf, BYTE head, int size) ;
void PHeadSubSetW(LPBYTE lpBuf, BYTE head, BYTE sub, int size);
void PHeadSetBE(LPBYTE lpBuf, BYTE head,int size);
void PHeadSubSetBE(LPBYTE lpBuf, BYTE head,BYTE sub, int size);
BOOL SpaceSyntexCheck(char* string);
BOOL QuoteSpaceSyntexCheck(char* string);
BOOL SQLSyntexCheck(char* SQLString);
DWORD MakeAccountKey(LPTSTR lpszAccountID);
#define CHECK_LIMIT(value, limit) (((value)< 0)?FALSE:((value)> limit-1)?FALSE:TRUE)

class char_ID
{

public:

	char_ID(LPSTR szName)	// line : 44
	{
		memset(Name, 0, sizeof(Name));

		if ( szName != NULL )
		{
			memcpy(Name, szName, MAX_ACCOUNT_LEN);
		}
	}	// line : 49

	LPSTR GetBuffer()	// line : 52
	{
		return Name;
	}	// line : 54

	int  GetLength()	// line : 63
	{
		return strlen(Name);
	}	// line : 65

private:

	char Name[MAX_ACCOUNT_LEN+1];	// 0

};





#endif