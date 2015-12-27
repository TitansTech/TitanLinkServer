// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// WinUtil.cpp
//	GS-N	1.00.18	JPN	0x004A4E40	-	Completed

#include "stdafx.h"
#include "winutil.h"
#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")
#include <rpc.h>
#include <rpcdce.h>
#pragma comment(lib, "rpcrt4.lib")
#include "../stdafx.h"

static unsigned char bBuxCode[3]	=	{0xFC, 0xCF, 0xAB};	

void BuxConvert(char* buf, int size)
{
	int n = 0;
	// ----
	for (n=0;n<size;n++)
	{
		buf[n]^=bBuxCode[n%3] ;		// Nice trick from WebZen
	}
}


void PHeadSetB(LPBYTE lpBuf, BYTE head, int size)
{
	lpBuf[0] =0xC1;		// Packets
	lpBuf[1] =size;
	lpBuf[2] =head;
}
	

void PHeadSubSetB(LPBYTE lpBuf, BYTE head, BYTE sub, int size)
{
	lpBuf[0] =0xC1;	// Packets
	lpBuf[1] =size;
	lpBuf[2] =head;
	lpBuf[3] =sub;
}

void PHeadSetW(LPBYTE lpBuf, BYTE head,  int size) 
{
	lpBuf[0] = 0xC2;	// Packets Header
	lpBuf[1]= SET_NUMBERH(size);
	lpBuf[2]= SET_NUMBERL(size);
	lpBuf[3]= head;
}

void PHeadSubSetW(LPBYTE lpBuf, BYTE head, BYTE sub,  int size)
{
	lpBuf[0]=0xC2;	// packet header
	lpBuf[1]=SET_NUMBERH(size);
	lpBuf[2]=SET_NUMBERL(size);
	lpBuf[3]=head;
	lpBuf[4]=sub;
}


void PHeadSetBE( LPBYTE lpBuf, BYTE head,int size)
{
	lpBuf[0] =0xC3;
	lpBuf[1] =size;
	lpBuf[2] =head;
}

void PHeadSubSetBE(LPBYTE lpBuf, BYTE head,BYTE sub, int size)
{
	lpBuf[0] =0xC3;
	lpBuf[1] =size;
	lpBuf[2] =head;
	lpBuf[3] =sub;
}

BOOL SpaceSyntexCheck(char* string)
{
	char* temp;
	
	temp = strchr(string, 32);
	if (temp ==0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL QuoteSpaceSyntexCheck(char* string)
{
     char* temp;
     
     temp = strchr(string, 39);
     if (temp ==0 )
     {
          return SpaceSyntexCheck(string);
     }
     else
     {
          return FALSE;
     }
}


BOOL SQLSyntexCheck(char* SQLString)
{
	char* temp;

	temp=strchr(SQLString, 39);
	if (temp==0)
	{
		temp=strchr(SQLString, 32);
		if (temp==0)
		{			
			temp=strchr(SQLString, 37);
			if(temp==0)
			{
				return TRUE;
			}else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

DWORD MakeAccountKey(LPTSTR lpszAccountID)
{
	int len = strlen(lpszAccountID);

	if ( len <= 0 )
		return 0;

	DWORD dwKey = 0;

	for(int i=0;i<len;i++)
		dwKey += lpszAccountID[i] + 17;
	
	dwKey = (dwKey + (10 - len) * 17) % 256;
	return dwKey % 256;
}