#include "stdafx.h"
#include "config.h"
#include "winutil.h"
#include "DSProtocol.h"
#include "PSProtocol.h"
#include "SCFExDBProtocol.h"
#include "ExServerProtocol.h"
#include "ExDBProtocol.h"
#include "JSProtocol.h"
#include "ServerManager.h"
#include "DB/Query.h"
#include "../stdafx.h"
#include "Log.h"

cDataServer DataServer1;
cDataServer DataServer2;
cPostServer PostServer;
cWzExDBServer WzExDBServer;
cJoinServer JoinServer;
cExtraServer ExtraServer;
BYTE JSCaseSensitive = 0;

/*
--- DATASERVER ---
*/

cDataServer::cDataServer()
{
	this->ServerCount = 0;
}

void cDataServer::Initialize(char * sName)
{
	if(strlen(sName) > sizeof(this->m_Name))
		strncpy(this->m_Name,sName,sizeof(this->m_Name));
	else
		strcpy(this->m_Name,sName);

	char sFileName[200]={0};
	wsprintf(sFileName,"./Config.ini");

	this->m_ServerPort = GetPrivateProfileInt(sName, "Port", 55962, sFileName);	
	if(this->m_ServerPort != -1)
	{
		int m_MaxConnections = GetPrivateProfileInt(sName, "MaxConnections", 10, sFileName);	
		GetPrivateProfileString(sName,"ServerODBC","MuOnline",this->m_ODBC,sizeof(this->m_ODBC),sFileName);
		GetPrivateProfileString(sName,"Name","MuOnline",this->m_DBName,sizeof(this->m_DBName),sFileName);
		GetPrivateProfileString(sName,"NameMe","MuOnline",this->m_DBName2,sizeof(this->m_DBName2),sFileName);
		GetPrivateProfileString(sName,"NameRS","MuOnline",this->m_DBName3,sizeof(this->m_DBName3),sFileName);
		GetPrivateProfileString(sName,"NameES","MuOnline",this->m_DBName4,sizeof(this->m_DBName4),sFileName);
		GetPrivateProfileString(sName,"ResetColumn","Resets",this->m_DBResets,sizeof(this->m_DBResets),sFileName);
		GetPrivateProfileString(sName,"Login","sa",this->m_DBLogin,sizeof(this->m_DBLogin),sFileName);
		GetPrivateProfileString(sName,"Password","654321",this->m_DBPassword,sizeof(this->m_DBPassword),sFileName);

		/* Database */
		this->vLogAdd("Initializing database connection");
		this->InitDB();	
		this->vLogAdd("Connected to %s@%s database", this->m_DBName, this->m_ODBC);
		this->vLogAdd("Initialized database connection");

		
		this->vLogAdd("Initializing IOCP system connection");
		this->Init(this->m_ServerPort,m_MaxConnections);
		this->vLogAdd("Initialized IOCP system connection");
	}
}

void cDataServer::ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize)
{
	DSProtocolCore(this,aIndex,HeadCode,aRecv,iSize);
}

void cDataServer::SendClearCoins()
{
	ClearCoins(this);
}

void cDataServer::SendGensRankingUpdate()
{
	GensRankingUpdate(this);
}

int DSCheckNumber()
{
	if(DataServer1.m_ServerPort != -1)
		return 1;
	else if(DataServer2.m_ServerPort != -1)
		return 2;
	else return 0;
}


void cDataServer::vLogAdd(char* szLog,...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAdd("[%s] %s",this->m_Name,szBuffer);
}

void cDataServer::vLogAddC(BYTE Color, char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddC(Color,"[%s] %s",this->m_Name,szBuffer);
}

void cDataServer::vLogAddTD(char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddTD("[%s] %s",this->m_Name,szBuffer);
}

void cDataServer::OnConnected(int aIndex)
{
}

void cDataServer::OnConnect(int aIndex)
{
	this->ServerCount++;
}

void cDataServer::OnDisconnect(int aIndex)
{
	this->ServerCount--;
}


void cDataServer::InitDB()
{
	if ( this->db.Connect(this->m_ODBC, this->m_DBLogin, this->m_DBPassword) == TRUE )
	{
		this->vLogAddC(4, "[DB] Connection Successfull!");
	}
	else
	{
		this->vLogAddC(2, "[DB] Error On Connection!");
		PostQuitMessage(0);
	}
}

/*
--- POSTSERVER ---
*/


cPostServer::cPostServer()
{
	this->ServerCount = 0;
}

void cPostServer::InitDB()
{
	if ( this->db.Connect(this->m_ODBC, this->m_DBLogin, this->m_DBPassword) == TRUE )
	{
		this->vLogAddC(4, "[DB] Connection Successfull!");
	}
	else
	{
		this->vLogAddC(2, "[DB] Error On Connection!");
		PostQuitMessage(0);
	}
}

void cPostServer::Initialize(char * sName)
{
	if(strlen(sName) > sizeof(this->m_Name))
		strncpy(this->m_Name,sName,sizeof(this->m_Name));
	else
		strcpy(this->m_Name,sName);

	char sFileName[200]={0};
	wsprintf(sFileName,"./Config.ini");

	this->m_ServerPort = GetPrivateProfileInt(sName, "Port", 55962, sFileName);	

	if(this->m_ServerPort != -1)
	{
		int m_MaxConnections = GetPrivateProfileInt(sName, "MaxConnections", 10, sFileName);	
		GetPrivateProfileString(sName,"AdminName","Admin",this->Admin_Name,sizeof(this->Admin_Name),sFileName);
		GetPrivateProfileString(sName,"ServerODBC","MuOnline",this->m_ODBC,sizeof(this->m_ODBC),sFileName);
		GetPrivateProfileString(sName,"Name","MuOnline",this->m_DBName,sizeof(this->m_DBName),sFileName);
		GetPrivateProfileString(sName,"Login","sa",this->m_DBLogin,sizeof(this->m_DBLogin),sFileName);
		GetPrivateProfileString(sName,"Password","654321",this->m_DBPassword,sizeof(this->m_DBPassword),sFileName);
		this->InitDB();
		this->vLogAdd("Initializing IOCP system connection");
		this->Init(this->m_ServerPort, m_MaxConnections);
		this->vLogAdd("Initialized IOCP system connection");
	}
}

void cPostServer::ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize)
{
	PSProtocolCore(this,aIndex,HeadCode,aRecv,iSize);
}

void cPostServer::vLogAdd(char* szLog,...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAdd("[%s] %s",this->m_Name,szBuffer);
}

void cPostServer::vLogAddC(BYTE Color, char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddC(Color,"[%s] %s",this->m_Name,szBuffer);
}

void cPostServer::vLogAddTD(char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddC(4,"[%s] %s",this->m_Name,szBuffer);
}

void cPostServer::OnConnected(int aIndex)
{
}

void cPostServer::OnConnect(int aIndex)
{
	this->ServerCount++;
}

void cPostServer::OnDisconnect(int aIndex)
{
	this->ServerCount--;
}

void cPostServer::SendAnnouncement(char * adminName, char * chat)
{
	SendAnnouncementPost(this,adminName,chat);
}

void cPostServer::SendAdmin(char * adminName, char * chat)
{
	SendAdminPost(this,adminName,chat);
}

/*
--- WzExDB ---
*/

cWzExDBServer::cWzExDBServer()
{
	this->ServerCount = 0;
}

void cWzExDBServer::Initialize(char * sName)
{
	if(strlen(sName) > sizeof(this->m_Name))
		strncpy(this->m_Name,sName,sizeof(this->m_Name));
	else
		strcpy(this->m_Name,sName);

	char sFileName[200]={0};
	wsprintf(sFileName,"./Config.ini");

	this->m_ServerPort = GetPrivateProfileInt(sName, "Port", 55906, sFileName);	
	if(this->m_ServerPort != -1)
	{
		int m_MaxConnections = GetPrivateProfileInt(sName, "MaxConnections", 10, sFileName);	
		GetPrivateProfileString(sName,"ServerODBC","MuOnline",this->m_ODBC,sizeof(this->m_ODBC),sFileName);
		GetPrivateProfileString(sName,"Name","MuOnline",this->m_DBName,sizeof(this->m_DBName),sFileName);
		GetPrivateProfileString(sName,"Login","sa",this->m_DBLogin,sizeof(this->m_DBLogin),sFileName);
		GetPrivateProfileString(sName,"Password","654321",this->m_DBPassword,sizeof(this->m_DBPassword),sFileName);

		/* Database */
		this->vLogAdd("Initializing database connection");
		this->InitDB();	
		this->vLogAdd("Connected to %s@%s database", this->m_DBName, this->m_ODBC);
		this->vLogAdd("Initialized database connection");

		
		this->vLogAdd("Initializing IOCP system connection");
		this->Init(this->m_ServerPort, m_MaxConnections);
		this->vLogAdd("Initialized IOCP system connection");

		for(int i=0;i<m_MaxConnections;i++)
		{
			this->eObj[i].m_State = SS_CLOSED;
			this->eObj[i].m_ServerCode = -1;
		}

		InitExDataServer(this);
	}
}

void cWzExDBServer::ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize)
{
	ExDBProtocolCore(this,aIndex,HeadCode,aRecv,iSize);
}

void cWzExDBServer::vLogAdd(char* szLog,...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAdd("[%s] %s",this->m_Name,szBuffer);
}

void cWzExDBServer::vLogAddC(BYTE Color, char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddC(Color,"[%s] %s",this->m_Name,szBuffer);
}

void cWzExDBServer::vLogAddTD(char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddTD("[%s] %s",this->m_Name,szBuffer);
}

void cWzExDBServer::OnConnected(int aIndex)
{
}

void cWzExDBServer::OnConnect(int aIndex)
{
	this->ServerCount++;
}

void cWzExDBServer::OnDisconnect(int aIndex)
{
	this->eObj[aIndex].m_State = SS_CLOSED;
	this->eObj[aIndex].m_ServerCode = -1;

	this->ServerCount--;
}


void cWzExDBServer::InitDB()
{
	if ( this->db.Connect(this->m_ODBC, this->m_DBLogin, this->m_DBPassword) == TRUE )
	{
		this->vLogAddC(4, "[DB] Connection Successfull!");
	}
	else
	{
		this->vLogAddC(2, "[DB] Error On Connection!");
		PostQuitMessage(0);
	}
}

/*
--- JOINSERVER ---
*/

cJoinServer::cJoinServer()
{
	this->ServerCount = 0;
}

void cJoinServer::Initialize(char * sName)
{
	if(strlen(sName) > sizeof(this->m_Name))
		strncpy(this->m_Name,sName,sizeof(this->m_Name));
	else
		strcpy(this->m_Name,sName);

	char sFileName[200]={0};
	wsprintf(sFileName,"./Config.ini");

	this->m_ServerPort = GetPrivateProfileInt(sName, "Port", 55970, sFileName);	
	if(this->m_ServerPort != -1)
	{
		int m_MaxConnections = MAX_LOGIN_SERVER;	
		GetPrivateProfileString(sName,"ServerODBC","MuOnlineJoinDB",this->m_ODBC,sizeof(this->m_ODBC),sFileName);
		GetPrivateProfileString(sName,"Name","Me_MuOnline",this->m_DBName,sizeof(this->m_DBName),sFileName);
		GetPrivateProfileString(sName,"Login","sa",this->m_DBLogin,sizeof(this->m_DBLogin),sFileName);
		GetPrivateProfileString(sName,"Password","654321",this->m_DBPassword,sizeof(this->m_DBPassword),sFileName);
		GetPrivateProfileString(sName,"CSIP","127.0.0.1",this->m_CSIP,sizeof(this->m_CSIP),sFileName);
		this->UDPPort				= GetPrivateProfileInt(sName, "CSUDPPort",55557,sFileName ) ;
		this->UseGoldChannel		= GetPrivateProfileInt(sName, "UseGoldChannelSystem",0,sFileName ) ;
		this->g_UseMD5				= GetPrivateProfileInt(sName, "UseMD5",1,sFileName ) ;
		this->UseSpecialDisconnect	= GetPrivateProfileInt(sName, "UseSpecialDisconnect",0,sFileName ) ;
		this->DisconnectInDB		= GetPrivateProfileInt(sName, "OnStartDisconnectAllInDB",0,sFileName ) ;
		this->SpecialMove			= GetPrivateProfileInt(sName, "UseSpecialMove",0,sFileName ) ;
		JSCaseSensitive				= GetPrivateProfileInt(sName, "UseCaseSensitive",0,sFileName);

		/* Database */
		this->vLogAdd("Initializing database connection");
		this->InitDB();	
		this->vLogAdd("Connected to %s@%s database", this->m_DBName, this->m_ODBC);
		this->vLogAdd("Initialized database connection");

		
		this->vLogAdd("Initializing IOCP system connection");
		this->Init(this->m_ServerPort,m_MaxConnections);
		this->vLogAdd("Initialized IOCP system connection");

		for(int i=0;i<m_MaxConnections;i++)
		{
			memset(this->eObj[i].m_szServerName , 0 , sizeof(this->eObj[i].m_szServerName));
			this->eObj[i].m_wPort = 0;
			this->eObj[i].m_wServerCode = -1;
			this->eObj[i].m_iVipCode = 0;
			this->eObj[i].m_GoldChannel = false;
		}

		for(int i = 0 ; i < MAX_LOGIN_USER ; i++)
		{
			memset(g_MuLoginUserData[i].m_AccoundID , 0 , sizeof(g_MuLoginUserData[i].m_AccoundID));
			g_MuLoginUserData[i].iUserNumber = -1;
			g_MuLoginUserData[i].m_bRequestMapSvrMove = false;
			g_MuLoginUserData[i].m_ServerCode = -1;
			g_MuLoginUserData[i].GoldTime = 0;
			memset(&g_MuLoginUserData[i].pMapMove , 0 , sizeof(g_MuLoginUserData[i].pMapMove));
		}
		g_ConnecServerUDP.CreateSocket();
		g_ConnecServerUDP.SendSet(this->m_CSIP, this->UDPPort);
		
		if(this->DisconnectInDB == 1)
		{
			this->db.ExecQuery("UPDATE MEMB_STAT SET ConnectStat = 0, DisConnectTM = getdate() WHERE ConnectStat = 1");
			this->db.Fetch();
			this->db.Close();
		}

		StartCheckGoldTime(this);
	}
}

void cJoinServer::ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize)
{
	JSProtocolCore(this,aIndex,HeadCode,aRecv,iSize);
}

void cJoinServer::vLogAdd(char* szLog,...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAdd("[%s] %s",this->m_Name,szBuffer);
}

void cJoinServer::vLogAddC(BYTE Color, char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddC(Color,"[%s] %s",this->m_Name,szBuffer);
}

void cJoinServer::vLogAddTD(char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddTD("[%s] %s",this->m_Name,szBuffer);
}

void cJoinServer::OnConnected(int aIndex)
{
}

void cJoinServer::OnConnect(int aIndex)
{
	this->ServerCount++;
}

void cJoinServer::OnDisconnect(int aIndex)
{
	this->ServerCount--;

	if(g_Config.UseTTCS == 0)
		CSSendServerInfo(this->eObj[aIndex].m_wServerCode,0);
	else
		CSSendServerInfo2(this->eObj[aIndex].m_wPort,0);

	for(int i = 0 ; i < MAX_LOGIN_USER ; i++)
	{
		if(g_MuLoginUserData[i].m_ServerCode == this->eObj[aIndex].m_wServerCode)
		{
			this->vLogAdd("[Object] %s has been deleted", g_MuLoginUserData[i].m_AccoundID);
			memset(g_MuLoginUserData[i].m_AccoundID, 0, 10);
			g_MuLoginUserData[i].m_ServerCode = -1;
			g_MuLoginUserData[i].m_bRequestMapSvrMove = false;
			g_MuLoginUserData[i].iUserNumber = -1;
			g_MuLoginUserData[i].GoldTime = 0;
			g_MuLoginUserDataCount--;
		}
	}

	if(this->eObj[aIndex].m_wServerCode > 0)
	{
		g_MuLoginServerDataCount--;
	}

	memset(this->eObj[aIndex].m_szServerName , 0 , sizeof(this->eObj[aIndex].m_szServerName));
	this->eObj[aIndex].m_wPort = 0;
	this->eObj[aIndex].m_wServerCode = -1;
	this->eObj[aIndex].m_iVipCode = 0;
	this->eObj[aIndex].m_GoldChannel = false;
}


void cJoinServer::InitDB()
{
	if ( this->db.Connect(this->m_ODBC, this->m_DBLogin, this->m_DBPassword) == TRUE )
	{
		this->vLogAddC(4, "[DB] Connection Successfull!");
	}
	else
	{
		this->vLogAddC(2, "[DB] Error On Connection!");
		PostQuitMessage(0);
	}
}

void cJoinServer::Statics()
{
	ShowStatics(this);
}

void cJoinServer::KickUser(char * AccountID)
{
	KickUserFunc(this,AccountID);
}


/*
--- EXTRASERVER ---
*/

cExtraServer::cExtraServer()
{
	this->ServerCount = 0;
}

void cExtraServer::Initialize(char * sName)
{
	if(strlen(sName) > sizeof(this->m_Name))
		strncpy(this->m_Name,sName,sizeof(this->m_Name));
	else
		strcpy(this->m_Name,sName);

	char sFileName[200]={0};
	wsprintf(sFileName,"./Config.ini");

	this->m_ServerPort = GetPrivateProfileInt(sName, "Port", 55999, sFileName);	
	if(this->m_ServerPort != -1)
	{
		int m_MaxConnections = GetPrivateProfileInt(sName, "MaxConnections", 10, sFileName);	
		GetPrivateProfileString(sName,"ServerODBC","MuOnline",this->m_ODBC,sizeof(this->m_ODBC),sFileName);
		GetPrivateProfileString(sName,"Name","MuOnline",this->m_DBName,sizeof(this->m_DBName),sFileName);
		GetPrivateProfileString(sName,"NameMe","MuOnline",this->m_DBName2,sizeof(this->m_DBName2),sFileName);
		GetPrivateProfileString(sName,"NameRS","Ranking",this->m_DBName3,sizeof(this->m_DBName3),sFileName);
		GetPrivateProfileString(sName,"Login","sa",this->m_DBLogin,sizeof(this->m_DBLogin),sFileName);
		GetPrivateProfileString(sName,"Password","654321",this->m_DBPassword,sizeof(this->m_DBPassword),sFileName);

		/* Database */
		this->vLogAdd("Initializing database connection");
		this->InitDB();	
		this->vLogAdd("Connected to %s@%s database", this->m_DBName, this->m_ODBC);
		this->vLogAdd("Initialized database connection");

		
		this->vLogAdd("Initializing IOCP system connection");
		this->Init(this->m_ServerPort,m_MaxConnections);
		this->vLogAdd("Initialized IOCP system connection");
	}
}

void cExtraServer::ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize)
{
	ExProtocolCore(this,aIndex,HeadCode,aRecv,iSize);
}

void cExtraServer::vLogAdd(char* szLog,...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAdd("[%s] %s",this->m_Name,szBuffer);
}

void cExtraServer::vLogAddC(BYTE Color, char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddC(Color,"[%s] %s",this->m_Name,szBuffer);
}

void cExtraServer::vLogAddTD(char* szLog, ...)
{
	char szBuffer[512]="";
	
	va_list pArguments;
	
	va_start( pArguments,  szLog);	// review
	vsprintf(szBuffer, szLog, pArguments);
	va_end(pArguments);

	LogAddTD("[%s] %s",this->m_Name,szBuffer);
}

void cExtraServer::OnConnected(int aIndex)
{
}

void cExtraServer::OnConnect(int aIndex)
{
	this->ServerCount++;
}

void cExtraServer::OnDisconnect(int aIndex)
{
	this->ServerCount--;
}


void cExtraServer::InitDB()
{
	if ( this->db.Connect(this->m_ODBC, this->m_DBLogin, this->m_DBPassword) == TRUE )
	{
		this->vLogAddC(4, "[DB] Connection Successfull!");
	}
	else
	{
		this->vLogAddC(2, "[DB] Error On Connection!");
		PostQuitMessage(0);
	}
}
