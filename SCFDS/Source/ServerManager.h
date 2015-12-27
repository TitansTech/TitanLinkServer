
#pragma once

#include "DB/Query.h"
#include "Network/cIOCP.h"

int DSCheckNumber();

class cDataServer : public cIOCP
{
public:
	cDataServer();
	void Initialize(char * sName);

	void vLogAdd(char* szLog,...);
	void vLogAddC(BYTE Color, char* szLog, ...);
	void vLogAddTD(char* szLog, ...);	
	void OnConnect(int aIndex);
	void OnConnected(int aIndex);
	void OnDisconnect(int aIndex);
	void ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize);

	void SendClearCoins();
	void SendGensRankingUpdate();
	
	CQuery db;
	
	int  m_ServerPort;
	char m_DBLogin[20];
	char m_DBPassword[20];
	char m_DBName[20];
	char m_ODBC[30];

	char m_DBName2[20];
	char m_DBName3[20];
	char m_DBName4[20];
	char m_DBResets[30];

private:
	void InitDB();

	char m_Name[20];
	int ServerCount;
};

extern cDataServer DataServer1;
extern cDataServer DataServer2;


class cPostServer : public cIOCP
{
public:
	cPostServer();
	void Initialize(char * sName);

	void vLogAdd(char* szLog,...);
	void vLogAddC(BYTE Color, char* szLog, ...);
	void vLogAddTD(char* szLog, ...);	
	void OnConnect(int aIndex);
	void OnConnected(int aIndex);
	void OnDisconnect(int aIndex);
	void ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize);
	
	void SendAnnouncement(char * adminName, char * chat);
	void SendAdmin(char * adminName, char * chat);
	
	CQuery db;

	int  m_ServerPort;
	char m_DBLogin[20];
	char m_DBPassword[20];
	char m_DBName[20];
	char m_ODBC[30];

	char m_DBName2[20];
	char m_DBName3[20];
	char m_DBName4[20];
	char m_DBResets[30];

	char Admin_Name[11];

private:
	void InitDB();

	char m_Name[20];

	int ServerCount;
};

extern cPostServer PostServer;


/*
--- WzExDB ---
*/

enum SERVER_STATE
{
	SS_CLOSED,
	SS_CONNECTED,
	SS_LOGGED,
	SS_GUILD,
	SS_CHAT,
};

struct EXTRAOBJ
{
	SERVER_STATE m_State;
	int m_ServerCode;
};

class cWzExDBServer : public cIOCP
{
public:
	cWzExDBServer();
	void Initialize(char * sName);

	void vLogAdd(char* szLog,...);
	void vLogAddC(BYTE Color, char* szLog, ...);
	void vLogAddTD(char* szLog, ...);	
	void OnConnect(int aIndex);
	void OnConnected(int aIndex);
	void OnDisconnect(int aIndex);
	void ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize);
	
	CQuery db;
	
	int  m_ServerPort;
	char m_DBLogin[20];
	char m_DBPassword[20];
	char m_DBName[20];
	char m_ODBC[30];

	EXTRAOBJ eObj[sOBJMAX];

private:
	void InitDB();

	char m_Name[20];
	int ServerCount;
};

extern cWzExDBServer WzExDBServer;

/*
--- JoinServer ---
*/


struct JSEXTRAOBJ
{
	char m_szServerName[50];
	int m_wServerCode;
	WORD m_wPort;
	WORD m_wCurrentUser;
	WORD m_wMaxUser;
	int m_iVipCode;
	bool m_GoldChannel;
};


class cJoinServer : public cIOCP
{
public:
	cJoinServer();
	void Initialize(char * sName);

	void vLogAdd(char* szLog,...);
	void vLogAddC(BYTE Color, char* szLog, ...);
	void vLogAddTD(char* szLog, ...);	
	void OnConnect(int aIndex);
	void OnConnected(int aIndex);
	void OnDisconnect(int aIndex);
	void ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize);
	void KickUser(char * AccountID);
	void Statics();
	
	CQuery db;
	
	int  m_ServerPort;
	char m_DBLogin[20];
	char m_DBPassword[20];
	char m_DBName[20];
	char m_ODBC[30];
	char m_CSIP[30];
	int  UDPPort;
	BYTE UseGoldChannel;
	BYTE g_UseMD5;
	BYTE UseSpecialDisconnect;
	BYTE DisconnectInDB;
	BYTE SpecialMove;
	BYTE CaseSensitive;

	JSEXTRAOBJ eObj[sOBJMAX];

private:
	void InitDB();

	char m_Name[20];
	int ServerCount;
};

extern cJoinServer JoinServer;
extern BYTE JSCaseSensitive;

/*
-- ExServer --
*/

class cExtraServer : public cIOCP
{
public:
	cExtraServer();
	void Initialize(char * sName);

	void vLogAdd(char* szLog,...);
	void vLogAddC(BYTE Color, char* szLog, ...);
	void vLogAddTD(char* szLog, ...);	
	void OnConnect(int aIndex);
	void OnConnected(int aIndex);
	void OnDisconnect(int aIndex);
	void ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize);
	
	CQuery db;
	
	int  m_ServerPort;
	char m_DBLogin[20];
	char m_DBPassword[20];
	char m_DBName[20];
	char m_ODBC[30];

	char m_DBName2[20];
	char m_DBName3[20];

private:
	void InitDB();

	char m_Name[20];
	int ServerCount;
};

extern cExtraServer ExtraServer;