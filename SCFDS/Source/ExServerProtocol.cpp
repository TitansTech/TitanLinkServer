#include "stdafx.h"
#include "SCFExDBProtocol.h"
#include "config.h"
#include "winutil.h"
#include "DSProtocol.h"
#include "../stdafx.h"
#include "Log.h"
#include "ExServerProtocol.h"
#include "../SCFExDB_Defines.h"
//#include "ServerManager.h"


void ExProtocolCore(cExtraServer * sManager, int aIndex, BYTE Head, LPBYTE aRecv, int aLen)
{
	switch (Head)
	{
		case 0x00:
			ExtraServerLogin(sManager, aIndex, (SDHP_SERVERINFO *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_SetPlayerQuestSave:
			GJSetPlayerS5QuestInfo(sManager, aIndex, (SDHP_GETPLAYERS5QUEST_INFOSAVE *) aRecv);
			break;
		case SCFExDB_GSSend_DeletePlayerQ5Info:
			GJDeletePlayerQ5Info(sManager, aIndex, (SDHP_DELETEPLAYERS5QUEST_INFO *) aRecv);
			break;
		case SCFExDB_GSSend_GetPlayerQ5Info:
			GDGetPlayerS5Quests(sManager, aIndex, (SDHP_GETPLAYEREXTRAINFO *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_GetPlayerCustomQuestsInfo:
			GDGetPlayerCustomQuests(sManager, aIndex, (SDHP_GETPLAYEREXTRAINFO *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_SendPlayerCustomQuestsInfo:
			GDSetPlayerCustomQuests(sManager, (SDHP_GETPLAYERCUSTOMQUESTS_SAVE *) aRecv);
			break;	
		//Update GoblinCoin
		case SCFExDB_GSSend_CShopAddGoblinCoin:
			MuItemShopUpdateGoblinCoin(sManager, aIndex, (SDHP_ADDGOBLINCOIN *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_CShopDataSend:
			MuItemShopBuyRecv(sManager, aIndex, (SDHP_MUITEMSHOPBUY *) aRecv);
			break;
		case SCFExDB_GSSend_CShopDataGiftSend:
			MuItemShopGiftBuyRecv(sManager, aIndex, (SDHP_MUITEMSHOPGIFTBUY *) aRecv);
			break;
		case SCFExDB_GSSend_CShopDataListSave_Normal:
			MuItemShopSaveList_Normal(sManager, aIndex, (SDHP_MUSHOPLISTSAVE_Normal *) aRecv);
			break;		
		case SCFExDB_GSSend_CShopDataListSave_Gift:
			MuItemShopSaveList_Gift(sManager, aIndex, (SDHP_MUSHOPLISTSAVE_Gift *) aRecv);
			break;
		case SCFExDB_GSSend_CShopDataListSave_Points:
			MuItemShopSaveList_Points(sManager, aIndex, (SDHP_MUSHOPLISTSAVE_Points *) aRecv);
			break;
		case SCFExDB_GSSend_CShopMuItemShopGetList:
			MuItemShopGetList(sManager, aIndex, (SDHP_GETACCOUNTEXTRAINFO *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_GetPlayerOfficialHelperInfo:
			GDGetPlayerOfficialHelper(sManager, aIndex, (SDHP_GETPLAYEREXTRAINFO *)aRecv);
			break;
		case SCFExDB_GSSend_SetOfficialHelperData:
			GDSetOfficialHelper(sManager, (SDHP_GETPLAYEROFFICIALHELPER_SAVE *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_DuelScore:
			DuelScore(sManager, (PMSG_ANS_DUELSCORE *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_DGScore:
			DGScore(sManager, (PMSG_ANS_DGSCORE *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_IGScore:
			IGScore(sManager, (PMSG_ANS_IGSCORE *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_MonsterKilled:
			MonstersKilledScore(sManager, (PMSG_ANS_MONSTERKILLED *)aRecv);
			break;
	}
}


void MonstersKilledScore(cExtraServer * sManager,PMSG_ANS_MONSTERKILLED * lpMsg)
{
	char szAccountID[11]			= {0};
	char szName[11]					= {0};

	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->Name, 10);

	sManager->db.ExecQuery("EXEC %s.dbo.TT_MonstersKilledScore %d, '%s', '%s', %d",
		sManager->m_DBName3,lpMsg->ServerCode,szAccountID,szName,lpMsg->Monster);
	sManager->db.Fetch();
	sManager->db.Close();
}


void IGScore(cExtraServer * sManager,PMSG_ANS_IGSCORE * lpMsg)
{
	char szAccountID[11]			= {0};
	char szName[11]					= {0};

	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->Name, 10);

	sManager->db.ExecQuery("EXEC %s.dbo.TT_IGScore %d, '%s', '%s', %d",
		sManager->m_DBName3,lpMsg->ServerCode,szAccountID,szName,lpMsg->Day);
	sManager->db.Fetch();
	sManager->db.Close();
}


void DGScore(cExtraServer * sManager,PMSG_ANS_DGSCORE * lpMsg)
{
	char szAccountID[11]			= {0};
	char szName[11]					= {0};

	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->Name, 10);

	sManager->db.ExecQuery("EXEC %s.dbo.TT_DGScore %d, '%s', '%s', %d, %d, %d",
		sManager->m_DBName3,lpMsg->ServerCode,szAccountID,szName,lpMsg->Result,lpMsg->MobPassed,lpMsg->MobToPass);
	sManager->db.Fetch();
	sManager->db.Close();
}

void DuelScore(cExtraServer * sManager,PMSG_ANS_DUELSCORE * lpMsg)
{
	char szAccountID1[11]			= {0};
	char szName1[11]				= {0};
	char szAccountID2[11]			= {0};
	char szName2[11]				= {0};

	memcpy(szAccountID1, lpMsg->AccountID_1, 10);
	memcpy(szName1, lpMsg->Name_1, 10);
	memcpy(szAccountID2, lpMsg->AccountID_2, 10);
	memcpy(szName2, lpMsg->Name_2, 10);

	int Average1 = lpMsg->Player1_Score - lpMsg->Player2_Score;
	int Average2 = Average1 * -1;
	
	BYTE Result1 = 1;
	BYTE Result2 = 1;

	if(Average1 < 0)
		Result1 = 0;

	if(Average2 < 0)
		Result2 = 0;


	sManager->db.ExecQuery("EXEC %s.dbo.TT_DuelScore %d, '%s', '%s', %d, %d",
		sManager->m_DBName3,lpMsg->ServerCode,szAccountID1,szName1,Result1,Average1);
	sManager->db.Fetch();
	sManager->db.Close();

	sManager->db.ExecQuery("EXEC %s.dbo.TT_DuelScore %d, '%s', '%s', %d, %d",
		sManager->m_DBName3,lpMsg->ServerCode,szAccountID2,szName2,Result2,Average2);
	sManager->db.Fetch();
	sManager->db.Close();
}

void GDSetPlayerCustomQuests(cExtraServer * sManager, SDHP_GETPLAYERCUSTOMQUESTS_SAVE * lpMsg)
{		
	char szName[11]={0};
	memcpy(szName, lpMsg->Name, 10);
		
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{
		char szTemp[10000];
		wsprintf(szTemp, "UPDATE %s.dbo.Character SET SCFCustomQuest=? WHERE Name='%s'",sManager->m_DBName, szName);
		sManager->db.SetAsBinary(szTemp, (LPBYTE)lpMsg->CustomQuest, sizeof(lpMsg->CustomQuest));
		sManager->db.Fetch();
		sManager->db.Close();
	}
}

void GDGetPlayerCustomQuests(cExtraServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * lpMsg)
{
	char Name[11] = {0};
	char AccountID[11] = {0};
	memcpy(Name, lpMsg->Name, 10);

	if((SQLSyntexCheck(Name) == TRUE)	&&
	   (SpaceSyntexCheck(Name) == TRUE))
	{
		SDHP_GETPLAYERCUSTOMQUESTS_SAVE pResult = {0};

		PHeadSetW((LPBYTE)&pResult, SCFExDB_GSRecv_GetPlayerCustomQuestsInfo, sizeof(pResult));
		memcpy(pResult.Name, Name, 10);	
		pResult.aIndex = lpMsg->aIndex;

		char szTemp[256];
		BYTE btTemp[CUSTOMQUEST_BINARY_SIZE]={0xFF};

		wsprintf(szTemp, "SELECT SCFCustomQuest FROM %s.dbo.Character where Name = '%s'",sManager->m_DBName, Name);
		if ( sManager->db.GetAsBinary(szTemp, btTemp) == -1 )
			memset(btTemp, -1, sizeof(btTemp));
		memcpy(pResult.CustomQuest, btTemp, sizeof(pResult.CustomQuest));
		sManager->db.Close();

		sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
	}	
}

void ExtraServerLogin(cExtraServer * sManager, int aIndex, SDHP_SERVERINFO * lpMsg)
{
	SDHP_RESULT pResult = {0};
	// ----
	pResult.h.set(0x00, sizeof(pResult));
	pResult.Result = 1;
	
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}

void MuItemShopBuyRecv(cExtraServer * sManager, int aIndex, SDHP_MUITEMSHOPBUY * aRecv)
{
	//OutputDebugString("ItemSerialCreateRecv\n");

	SDHP_MUITEMSHOPBUYRECV		pResult = {0};
	int							Serial2	= 0;
	// ----
	PHeadSetB((LPBYTE)&pResult, SCFExDB_GSRecv_CShopDataRecv, sizeof(pResult));
	// ----
	sManager->db.ExecQuery("EXEC TT_GetMuShopItemSerial");
	sManager->db.Fetch();
	// ----
	Serial2				= sManager->db.GetResult(0);
	// ----
	sManager->db.Close();
	// ----
	strcpy(pResult.AccountID,aRecv->AccountID);
	pResult.m_Serial2	= Serial2;
	pResult.aIndex		= aRecv->aIndex;
	pResult.ItemID		= aRecv->ItemID;
	pResult.ID1			= aRecv->ID1;
	pResult.ID2			= aRecv->ID2;
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}

//GoblinCoin fix for ChaosMachine mixes
void MuItemShopUpdateGoblinCoin(cExtraServer * sManager, int aIndex, SDHP_ADDGOBLINCOIN * aRecv)
{
	char AccountID[11] = {0};
	memcpy(AccountID, aRecv->AccountID, 10);

	int GoblinCoin = aRecv->GoblinCoin;

	if (GoblinCoin > 0)
	{
		sManager->db.ExecQuery("UPDATE %s.dbo.MEMB_INFO SET GoblinCoin=GoblinCoin+%d WHERE memb___id='%s'",
			sManager->m_DBName2, GoblinCoin, AccountID
		);
		sManager->db.Fetch();
		sManager->db.Close();
	}
}

void MuItemShopGetList(cExtraServer * sManager, int aIndex, SDHP_GETACCOUNTEXTRAINFO * aRecv)
{
	char AccountID[11] = {0};
	memcpy(AccountID, aRecv->AccountID, 10);

	SDHP_MUSHOPLISTSAVE pResult = {0};
	PHeadSetW((LPBYTE)&pResult, SCFExDB_GSRecv_CShopDataListRecv, sizeof(pResult));
	memcpy(pResult.AccountID, AccountID, 10);
	pResult.aIndex = aRecv->aIndex;
	
	char szTemp2[10000];
	BYTE btTemp2[MUSHOPLIST_SIZE]={0};
	BYTE btTemp3[MUSHOPLIST_MSG_SIZE]={0};

	wsprintf(szTemp2, "SELECT MuItemShopList FROM %s.dbo.MEMB_INFO where memb___id = '%s'",sManager->m_DBName2, AccountID);
	if ( sManager->db.GetAsBinary(szTemp2, btTemp2) == -1 )
		memset(btTemp2, 0, sizeof(btTemp2));
	
	memcpy(pResult.List, btTemp2, sizeof(pResult.List));
	sManager->db.Close();
	
	memset(btTemp2, 0, sizeof(btTemp2));

	wsprintf(szTemp2, "SELECT MuItemShopGiftList FROM %s.dbo.MEMB_INFO where memb___id = '%s'",sManager->m_DBName2, AccountID);
	if ( sManager->db.GetAsBinary(szTemp2, btTemp2) == -1 )
		memset(btTemp2, 0, sizeof(btTemp2));
	
	memcpy(pResult.GiftList, btTemp2, sizeof(pResult.GiftList));
	sManager->db.Close();
	

	wsprintf(szTemp2, "SELECT MuItemShopGiftMsg FROM %s.dbo.MEMB_INFO where memb___id = '%s'",sManager->m_DBName2, AccountID);
	if ( sManager->db.GetAsBinary(szTemp2, btTemp3) == -1 )
		memset(btTemp3, 0, sizeof(btTemp3));
	
	memcpy(pResult.GiftMsg, btTemp3, sizeof(pResult.GiftList));
	sManager->db.Close();


	sManager->db.ExecQuery("SELECT WCoin,WCoinP,GoblinCoin FROM %s.dbo.MEMB_INFO where memb___id= '%s'",
		sManager->m_DBName2,AccountID);
	if ( sManager->db.Fetch() != SQL_NO_DATA ) 
	{
		pResult.WCoin		= sManager->db.GetAsInteger("WCoin"); 
		pResult.WCoinP		= sManager->db.GetAsInteger("WCoinP"); 
		pResult.GoblinCoin	= sManager->db.GetAsInteger("GoblinCoin"); 
	}
	sManager->db.Close();


	sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
}

//void MuItemShopSaveList(cExtraServer * sManager, int aIndex, SDHP_MUSHOPLISTSAVE * lpMsg)
//{
//	char szTemp[10000];
//	wsprintf(szTemp, "UPDATE %s.dbo.MEMB_INFO SET MuItemShopList=? WHERE memb___id='%s'",sManager->m_DBName2, lpMsg->AccountID);
//	sManager->db.SetAsBinary(szTemp, (LPBYTE)lpMsg->List, sizeof(lpMsg->List));
//	sManager->db.Fetch();
//	sManager->db.Close();
//	
//	char szTemp2[10000];
//	wsprintf(szTemp2, "UPDATE %s.dbo.MEMB_INFO SET MuItemShopGiftList=? WHERE memb___id='%s'",sManager->m_DBName2, lpMsg->AccountID);
//	sManager->db.SetAsBinary(szTemp2, (LPBYTE)lpMsg->GiftList, sizeof(lpMsg->GiftList));
//	sManager->db.Fetch();
//	sManager->db.Close();
//	
//	char szTemp3[10000];
//	wsprintf(szTemp3, "UPDATE %s.dbo.MEMB_INFO SET MuItemShopGiftMsg=? WHERE memb___id='%s'",sManager->m_DBName2, lpMsg->AccountID);
//	sManager->db.SetAsBinary(szTemp3, (LPBYTE)lpMsg->GiftMsg, sizeof(lpMsg->GiftMsg));
//	sManager->db.Fetch();
//	sManager->db.Close();
//	
//	sManager->db.ExecQuery("UPDATE %s.dbo.MEMB_INFO SET WCoin=%d, WCoinP=%d, GoblinCoin=%d WHERE memb___id='%s'",
//		sManager->m_DBName2,lpMsg->WCoin, lpMsg->WCoinP, lpMsg->GoblinCoin, lpMsg->AccountID);
//
//	sManager->db.Fetch();
//	sManager->db.Close();
//}

void MuItemShopSaveList_Normal(cExtraServer * sManager, int aIndex, SDHP_MUSHOPLISTSAVE_Normal * lpMsg)
{
	char szTemp[10000];
	wsprintf(szTemp, "UPDATE %s.dbo.MEMB_INFO SET MuItemShopList=? WHERE memb___id='%s'",sManager->m_DBName2, lpMsg->AccountID);
	sManager->db.SetAsBinary(szTemp, (LPBYTE)lpMsg->List, sizeof(lpMsg->List));
	sManager->db.Fetch();
	sManager->db.Close();
}

void MuItemShopSaveList_Gift(cExtraServer * sManager, int aIndex, SDHP_MUSHOPLISTSAVE_Gift * lpMsg)
{	
	char szTemp2[10000];
	wsprintf(szTemp2, "UPDATE %s.dbo.MEMB_INFO SET MuItemShopGiftList=? WHERE memb___id='%s'",sManager->m_DBName2, lpMsg->AccountID);
	sManager->db.SetAsBinary(szTemp2, (LPBYTE)lpMsg->GiftList, sizeof(lpMsg->GiftList));
	sManager->db.Fetch();
	sManager->db.Close();
	
	char szTemp3[10000];
	wsprintf(szTemp3, "UPDATE %s.dbo.MEMB_INFO SET MuItemShopGiftMsg=? WHERE memb___id='%s'",sManager->m_DBName2, lpMsg->AccountID);
	sManager->db.SetAsBinary(szTemp3, (LPBYTE)lpMsg->GiftMsg, sizeof(lpMsg->GiftMsg));
	sManager->db.Fetch();
	sManager->db.Close();
}

void MuItemShopSaveList_Points(cExtraServer * sManager, int aIndex, SDHP_MUSHOPLISTSAVE_Points * lpMsg)
{		
	sManager->db.ExecQuery("UPDATE %s.dbo.MEMB_INFO SET WCoin=%d, WCoinP=%d, GoblinCoin=%d WHERE memb___id='%s'",
		sManager->m_DBName2,lpMsg->WCoin, lpMsg->WCoinP, lpMsg->GoblinCoin, lpMsg->AccountID);

	sManager->db.Fetch();
	sManager->db.Close();
}


void MuItemShopGiftBuyRecv(cExtraServer * sManager, int aIndex, SDHP_MUITEMSHOPGIFTBUY * aRecv)
{
	//OutputDebugString("ItemSerialCreateRecv\n");

	SDHP_MUITEMSHOPGIFTBUY		pResult = {0};
	int							Serial2	= 0;
	// ----
	PHeadSetB((LPBYTE)&pResult, SCFExDB_GSRecv_CShopDataGiftRecv, sizeof(pResult));
	// ----
	sManager->db.ExecQuery("EXEC TT_GetMuShopItemSerial");
	sManager->db.Fetch();
	// ----
	Serial2				= sManager->db.GetResult(0);
	// ----
	sManager->db.Close();
	// ----
	strcpy(pResult.AccountID,aRecv->AccountID);
	pResult.m_Serial2	= Serial2;
	pResult.aIndex		= aRecv->aIndex;
	pResult.ItemID		= aRecv->ItemID;
	pResult.ID1			= aRecv->ID1;
	pResult.ID2			= aRecv->ID2;
	strcpy(pResult.Name,aRecv->Name);
	strcpy(pResult.Msg,aRecv->Msg);
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}

void GDGetPlayerS5Quests(cExtraServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * lpMsg)
{
	char Name[11] = {0};
	char AccountID[11] = {0};
	memcpy(Name, lpMsg->Name, 10);
	memcpy(AccountID, lpMsg->AccountID, 10);

	if((SQLSyntexCheck(Name) == TRUE)	&&
	   (SpaceSyntexCheck(Name) == TRUE))
	{
		SDHP_GETPLAYERS5QUEST_SAVE pResultS5Q = {0};
		PHeadSetW((LPBYTE)&pResultS5Q, SCFExDB_GSRecv_GetPlayerS5QuestInfo, sizeof(pResultS5Q));

		char szTemp2[10000];
		BYTE btTemp2[(sizeof(S5QUEST)*MAX_QUEST_COUNT)]={0};

		memcpy(pResultS5Q.AccountID, AccountID, 10);
		memcpy(pResultS5Q.Name, Name, 10);	
		pResultS5Q.aIndex = lpMsg->aIndex;

		BOOL bRet = sManager->db.ExecQuery("SELECT Name FROM %s.dbo.SCFS5Quest WHERE Name = '%s' and AccountID = '%s'",sManager->m_DBName, Name, AccountID);
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			memset(btTemp2, 0, sizeof(btTemp2));
		} else 
		{
			sManager->db.Close();
			wsprintf(szTemp2, "SELECT SCFS5Quest FROM %s.dbo.SCFS5Quest where Name = '%s' and AccountID = '%s'",sManager->m_DBName, Name, AccountID);
			if ( sManager->db.GetAsBinary(szTemp2, btTemp2) == -1 )
				memset(btTemp2, 0, sizeof(btTemp2));
		}
		memcpy(pResultS5Q.S5Quest, btTemp2, sizeof(pResultS5Q.S5Quest));
		sManager->db.Close();
		sManager->DataSend(aIndex, (LPBYTE)&pResultS5Q, sizeof(pResultS5Q));
	}
}

void GJDeletePlayerQ5Info(cExtraServer * sManager, int aIndex, SDHP_DELETEPLAYERS5QUEST_INFO * pRecv)
{
	char szName[11]={0};
	memcpy(szName, pRecv->Name, 10);

	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{
		sManager->db.ExecQuery("DELETE FROM %s.dbo.SCFS5Quest WHERE Name = '%s' and AccountID= '%s'",
			sManager->m_DBName, pRecv->Name, pRecv->AccountID);
		sManager->db.Fetch();
		sManager->db.Close();
	}
}

void GJSetPlayerS5QuestInfo(cExtraServer * sManager, int aIndex, SDHP_GETPLAYERS5QUEST_INFOSAVE * lpMsg)
{
	char szName[11]={0};
	memcpy(szName, lpMsg->Name, 10);

	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{

		BOOL bRet = sManager->db.ExecQuery("SELECT Name FROM %s.dbo.SCFS5Quest WHERE Name = '%s' and AccountID= '%s'",sManager->m_DBName, lpMsg->Name, lpMsg->AccountID);
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			sManager->db.Close();
			sManager->db.ExecQuery("INSERT INTO %s.dbo.SCFS5Quest (AccountID, Name) VALUES ('%s','%s')",sManager->m_DBName, lpMsg->AccountID, lpMsg->Name);
			sManager->db.Fetch();
		}
		sManager->db.Close();

		char szTemp[10000];
		wsprintf(szTemp, "UPDATE %s.dbo.SCFS5Quest SET SCFS5Quest=? WHERE Name='%s' and AccountID='%s'",sManager->m_DBName, lpMsg->Name, lpMsg->AccountID);
		sManager->db.SetAsBinary(szTemp, (LPBYTE)lpMsg->S5Quest, sizeof(lpMsg->S5Quest));
		sManager->db.Fetch();
		sManager->db.Close();
	}
}



void GDGetPlayerOfficialHelper(cExtraServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * lpMsg)
{
	char Name[11] = {0};
	char AccountID[11] = {0};
	memcpy(Name, lpMsg->Name, 10);
	memcpy(AccountID, lpMsg->AccountID, 10);

	if((SQLSyntexCheck(Name) == TRUE)	&&
	   (SpaceSyntexCheck(Name) == TRUE))
	{
		SDHP_GETPLAYEROFFICIALHELPER_SAVE pResultOfficialHelper = {0};
		PHeadSetW((LPBYTE)&pResultOfficialHelper, SCFExDB_GSRecv_GetOfficialHelperData, sizeof(pResultOfficialHelper));
		char szTemp3[256];
		BYTE btTemp3[257]={0};

		memcpy(pResultOfficialHelper.AccountID, AccountID, 10);
		memcpy(pResultOfficialHelper.Name, Name, 10);	
		pResultOfficialHelper.aIndex = lpMsg->aIndex;

		BOOL bRet = sManager->db.ExecQuery("SELECT Name FROM %s.dbo.OfficialHelper WHERE Name = '%s'",sManager->m_DBName, Name);
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			memset(btTemp3, 0, sizeof(btTemp3));
			btTemp3[1] = 1;
			btTemp3[2] = 0x11;
			btTemp3[23] = 0x51;
			btTemp3[24] = 0x15;
			btTemp3[26] = 0x20;
		}else
		{
			sManager->db.Close();
			wsprintf(szTemp3, "SELECT Data FROM %s.dbo.OfficialHelper where Name = '%s'",sManager->m_DBName, Name);
			if ( sManager->db.GetAsBinary(szTemp3, btTemp3) == -1 )
				memset(btTemp3, 0, sizeof(btTemp3));
		}
		memcpy(pResultOfficialHelper.Data, btTemp3, sizeof(pResultOfficialHelper.Data));
		sManager->db.Close();
		sManager->DataSend(aIndex, (LPBYTE)&pResultOfficialHelper, sizeof(pResultOfficialHelper));
	}
}

void GDSetOfficialHelper(cExtraServer * sManager, SDHP_GETPLAYEROFFICIALHELPER_SAVE * lpMsg)
{
	char szName[11]={0};
	memcpy(szName, lpMsg->Name, 10);
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{
		BOOL bRet = sManager->db.ExecQuery("SELECT Name FROM %s.dbo.OfficialHelper WHERE Name = '%s'",sManager->m_DBName, lpMsg->Name);
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			sManager->db.Close();
			sManager->db.ExecQuery("INSERT INTO %s.dbo.OfficialHelper (Name) VALUES ('%s')",sManager->m_DBName, lpMsg->Name);
			sManager->db.Fetch();
			sManager->db.Close();
			char szTemp2[10000];
			wsprintf(szTemp2, "UPDATE %s.dbo.OfficialHelper SET Data=? WHERE Name='%s'",sManager->m_DBName, szName);
			sManager->db.SetAsBinary(szTemp2, (LPBYTE)lpMsg->Data, sizeof(lpMsg->Data));
			sManager->db.Fetch();
		}else
		{
			char szTemp2[10000];
			wsprintf(szTemp2, "UPDATE %s.dbo.OfficialHelper SET Data=? WHERE Name='%s'",sManager->m_DBName, szName);
			sManager->db.SetAsBinary(szTemp2, (LPBYTE)lpMsg->Data, sizeof(lpMsg->Data));
			sManager->db.Fetch();
		}
		sManager->db.Close();
	}
}
