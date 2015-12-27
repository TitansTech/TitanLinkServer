// Query.h: interface for the CQuery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_)
#define AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../Network/TSynchro.h"

#define MAX_COLUMNS	100

class CQuery  
{
public:

	CQuery();
	virtual ~CQuery();

	BOOL Connect(LPTSTR lpszDNS, LPTSTR lpszUser, LPTSTR lpszPassword);
	BOOL ExecQuery(LPTSTR lpszStatement, ...);
	void Close();
	void GetAsString(LPTSTR ColName, LPTSTR pOutBuffer,int size);
	void GetAsString(LPTSTR ColName, LPTSTR pOutBuffer);
	DWORD GetAsInteger(LPTSTR ColName);
	__int64	GetAsInteger64(LPTSTR ColName);
	float GetAsFloat(LPTSTR ColName);
	void Disconnect();
	int GetAsBinary(LPSTR lpszStatement, LPBYTE OUT lpszReturnBuffer);
	SQLRETURN Fetch();
	int GetResult(int iIndex);
	
#ifdef _WIN64
	void SetAsBinary(LPTSTR lpszStatement, LPBYTE lpBinaryBuffer, SQLLEN BinaryBufferSize);
#else
	void SetAsBinary(LPTSTR lpszStatement, LPBYTE lpBinaryBuffer, SQLUINTEGER BinaryBufferSize);
#endif

private:

	BOOL ReConnect();
	
	int FindIndex(LPTSTR ColName);
	void Diagnosis();

	//CRITICAL_SECTION m_criti;
	TSynchroCriticalSection Sync;
	bool b_criti;

protected:

	SQLHANDLE m_hEnviroment;
	SQLHANDLE m_hConnection;
	SQLHANDLE m_hStmt;
	TCHAR m_szDNS[64];
	TCHAR m_szUser[64];
	TCHAR m_szPassword[64];
	SQLSMALLINT m_ColCount;
	SQLTCHAR m_SQLColName[MAX_COLUMNS][30];
	TCHAR m_SQLData[MAX_COLUMNS][256];
#ifdef _WIN64
	SQLLEN m_SQLDataLen[MAX_COLUMNS];
#else
	SQLINTEGER m_SQLDataLen[MAX_COLUMNS];
#endif
};

#endif // !defined(AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_)
