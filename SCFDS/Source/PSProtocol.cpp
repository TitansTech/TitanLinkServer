#include "stdafx.h"
#include "config.h"
#include "winutil.h"
#include "PSProtocol.h"
#include "../stdafx.h"
#include "Log.h"
#include "DSProtocol.h"
#include "SCFExDBProtocol.h"
#include "RProtocol.h"
#include "EProtocol.h"
#include "../SCFExDB_Defines.h"
#include "ExServerProtocol.h"

void PSProtocolCore(cPostServer * sManager, int aIndex, BYTE Head, LPBYTE pRecv, int iSize)
{
	switch(Head)
	{
		case 0x00:
			PSDataServerLogin(sManager, aIndex, (SDHP_SERVERINFO *)pRecv);
			break;
		case 0x01:
			GetPost(sManager, aIndex, (SDHP_POSTINFO *)pRecv);
			break;
		case 0x02:
			GetAllSvrMsg(sManager, aIndex, (SDHP_ALLSVRMSGSENDINFO *)pRecv);
			break;
	}
}

/*
SELECT TOP 1000 [id]
      ,[ServerCode]
      ,[PlayerName]
      ,[Message]
  FROM [MuOnline].[dbo].[PostserverLogs]
*/
void PSDataInsert(cDataServer * sManager, int aIndex, SDHP_POSTINFO * lpMsg)
{
	SDHP_POSTINFO pResult = {0};
	memcpy(&pResult,(LPBYTE)lpMsg,sizeof(pResult));	

	sManager->db.ExecQuery("INSERT INTO MuOnline.dbo.PostserverLogs (id, ServerCode, PlayerName, Message) VALUES ('', %d, %s, %s)", lpMsg->ServerCode, lpMsg->Name, lpMsg->Chat);
	sManager->db.Fetch();
	sManager->db.Close();
}

void PSDataServerLogin(cPostServer * sManager, int aIndex, SDHP_SERVERINFO * lpMsg)
{
	SDHP_RESULT pResult = {0};

	PHeadSetB((LPBYTE)&pResult, 0x00, sizeof(pResult));
	pResult.Result = 1;
	
	sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
	sManager->vLogAddC(4,"[PostServer][%d] Connection Success: Server Code %d",aIndex,lpMsg->ServerCode);
}

void GetPost(cPostServer * sManager, int aIndex, SDHP_POSTINFO * lpMsg)
{
	char *	Name	= 0;
	char *	Chat	= 0;

	Name = lpMsg->Name;
	Chat = lpMsg->Chat;

	SDHP_POSTINFO pResult = {0};
	memcpy(&pResult,(LPBYTE)lpMsg,sizeof(pResult));	

	memcpy(Name, lpMsg->Name, sizeof(lpMsg->Name));
	memcpy(Chat, lpMsg->Chat, sizeof(lpMsg->Chat));

	sManager->db.ExecQuery("INSERT INTO MuOnline.dbo.PostserverLogs (ServerCode, PlayerName, Message) VALUES ('%d', '%s', '%s')", lpMsg->ServerCode, Name, Chat);
	sManager->db.Fetch();
	sManager->db.Close();

	sManager->DataSendAll((LPBYTE)&pResult, sizeof(pResult));
	sManager->vLogAddC(4,"[PostServer][%d][GS:%d](%s) %s",aIndex,lpMsg->ServerCode,lpMsg->Name,lpMsg->Chat);
}

void GetAllSvrMsg(cPostServer * sManager, int aIndex, SDHP_ALLSVRMSGSENDINFO * lpMsg)
{
	SDHP_ALLSVRMSGSENDINFO pResult = {0};
	memcpy(&pResult,(LPBYTE)lpMsg,sizeof(pResult));
	sManager->DataSendAll((LPBYTE)&pResult, sizeof(pResult));
	sManager->vLogAddC(8,"[PostServer][%d][GS:%d](GM) %s",aIndex,lpMsg->ServerCode,lpMsg->Chat);
}

void SendAdminPost(cPostServer * sManager, char * adminName, char * chat)
{
	if(sManager->m_ServerPort != -1)
	{
		SDHP_POSTINFO pResult = {0};

		PHeadSetB((LPBYTE)&pResult, 0x01, sizeof(pResult));

		strncpy(pResult.Name,adminName,sizeof(pResult.Name));
		strncpy(pResult.Chat,chat,sizeof(pResult.Chat));

		pResult.ServerCode = -1;
		
		sManager->DataSendAll((LPBYTE)&pResult, sizeof(pResult));
		sManager->vLogAddC(8,"[PostServer][Post](GM) %s", pResult.Chat);
	}
}

void SendAnnouncementPost(cPostServer * sManager, char * adminName, char * chat)
{
	if(sManager->m_ServerPort != -1)
	{
		char sbuf[70]={0};
		int Len = strlen(adminName) + strlen(chat) + 3;
		if(Len > 60)
			Len = 60;
		wsprintf(sbuf,"[%s] %s",adminName,chat);
		
		SDHP_ALLSVRMSGSENDINFO pResult={0};
		PHeadSetB((LPBYTE)&pResult, 0x02, sizeof(pResult));
		strncpy(pResult.Chat,sbuf,Len);
		pResult.ServerCode = -1;
		sManager->DataSendAll((LPBYTE)&pResult, sizeof(pResult));
		sManager->vLogAddC(8,"[PostServer][Announce](GM) %s", pResult.Chat);
	}
}