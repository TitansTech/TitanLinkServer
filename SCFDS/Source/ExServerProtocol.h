
#pragma once

#include "ServerManager.h"
#include "SCFExDBProtocol.h"

#define MAX_QUEST_COUNT					100
#define MAX_QUEST_REQ					75

#define CUSTOMQUEST_BINARY_SIZE			512

#define MUSHOPLIST_SIZE					720
#define MUSHOPLIST_MSG_SIZE				4995
#define MUITEMSHOP_MAXITEMPERPAGE		9
#define MUITEMSHOP_MAXPAGES				5



struct SDHP_MUITEMSHOPBUYRECV
{
	PBMSG_HEAD h;
	char AccountID[11];
	short aIndex;
	DWORD m_Serial2;
	WORD ItemID;
	WORD ID1;
	WORD ID2;
};


struct SDHP_MUITEMSHOPBUY
{
	PBMSG_HEAD h;
	char AccountID[11];
	short aIndex;

	WORD ItemID;
	WORD ID1;
	WORD ID2;
};

struct MS_ANS_MSLISTDATA
{
	struct PWMSG_HEAD h;	// C2:53
	char AccountID[11];
	short aIndex;
	int iCount;				//
};

struct MS_MSLISTDATA
{	
	DWORD m_Serial1;
	DWORD m_Serial2;
	WORD ItemID;
	WORD ID1;
	WORD ID2;
};

struct SDHP_MUSHOPLISTSAVE
{
	PWMSG_HEAD h;	
	char AccountID[11];
	short aIndex;
	int WCoin;
	int WCoinP;
	int GoblinCoin;

	BYTE List[MUSHOPLIST_SIZE];	//
	BYTE GiftList[MUSHOPLIST_SIZE];	//
	BYTE GiftMsg[MUSHOPLIST_MSG_SIZE];	//
};


struct SDHP_MUSHOPLISTSAVE_Normal
{
	PWMSG_HEAD h;	
	char AccountID[11];

	BYTE List[MUSHOPLIST_SIZE];	//
};

struct SDHP_MUSHOPLISTSAVE_Gift
{
	PWMSG_HEAD h;	
	char AccountID[11];

	BYTE GiftList[MUSHOPLIST_SIZE];	//
	BYTE GiftMsg[MUSHOPLIST_MSG_SIZE];	//
};

struct SDHP_MUSHOPLISTSAVE_Points
{
	PBMSG_HEAD h;
	char AccountID[11];

	int WCoin;
	int WCoinP;
	int GoblinCoin;
};

struct SDHP_MUITEMSHOPGIFTBUY
{
	PBMSG_HEAD h;
	char AccountID[11];
	short aIndex;

	DWORD m_Serial2;
	WORD ItemID;
	WORD ID1;
	WORD ID2;
	char Name[11];
	char Msg[100];
};

struct SDHP_ADDGOBLINCOIN
{
	PBMSG_HEAD h;	// C1:01
	char AccountID[11];
	int GoblinCoin;
};

struct SDHP_GETCUSTOMQUEST
{
	BYTE Number;
	BYTE Status;
};


struct SDHP_GETPLAYERCUSTOMQUESTS_SAVE
{
	PWMSG_HEAD h;
	char Name[11];
	short aIndex;
	SDHP_GETCUSTOMQUEST CustomQuest[256];
};

struct S5QUEST
{
	BYTE Active;
	WORD NumID;
	BYTE State[MAX_QUEST_REQ];
};

struct SDHP_GETPLAYERS5QUEST_SAVE
{
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short aIndex;
	S5QUEST S5Quest[MAX_QUEST_COUNT];
};

struct SDHP_DELETEPLAYERS5QUEST_INFO
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
};


struct SDHP_GETPLAYERS5QUEST_INFOSAVE
{
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	S5QUEST S5Quest[MAX_QUEST_COUNT];
};


struct SDHP_GETPLAYEROFFICIALHELPER_SAVE
{
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short aIndex;
	BYTE Data[257];
};

struct PMSG_ANS_DUELSCORE
{
	PBMSG_HEAD h;
	int ServerCode;
	char AccountID_1[10];
	char Name_1[10];
	char AccountID_2[10];
	char Name_2[10];
	BYTE Player1_Score;
	BYTE Player2_Score;
};

struct PMSG_ANS_DGSCORE
{
	PBMSG_HEAD h;
	int ServerCode;
	char AccountID[10];
	char Name[10];
	BYTE MobPassed;
	BYTE MobToPass;
	BYTE Result;
};

struct PMSG_ANS_IGSCORE
{
	PBMSG_HEAD h;
	int ServerCode;
	char AccountID[10];
	char Name[10];
	BYTE Day;
};


struct PMSG_ANS_MONSTERKILLED
{
	PBMSG_HEAD h;
	int ServerCode;
	char AccountID[10];
	char Name[10];
	WORD Monster;
};


void MuItemShopUpdateGoblinCoin(cExtraServer * sManager, int aIndex, SDHP_ADDGOBLINCOIN * aRecv);
void ExtraServerLogin(cExtraServer * sManager, int aIndex, SDHP_SERVERINFO * lpMsg);
void ExProtocolCore(cExtraServer * sManager, int aIndex, BYTE Head, LPBYTE aRecv, int aLen);
void MuItemShopGetList(cExtraServer * sManager, int aIndex, SDHP_GETACCOUNTEXTRAINFO * aRecv);
void MuItemShopBuyRecv(cExtraServer * sManager, int aIndex, SDHP_MUITEMSHOPBUY * aRecv);
void MuItemShopGiftBuyRecv(cExtraServer * sManager, int aIndex, SDHP_MUITEMSHOPGIFTBUY * aRecv);
void MuItemShopSaveList_Points(cExtraServer * sManager, int aIndex, SDHP_MUSHOPLISTSAVE_Points * aRecv);
void MuItemShopSaveList_Normal(cExtraServer * sManager, int aIndex, SDHP_MUSHOPLISTSAVE_Normal * aRecv);
void MuItemShopSaveList_Gift(cExtraServer * sManager, int aIndex, SDHP_MUSHOPLISTSAVE_Gift * aRecv);
void GDSetPlayerCustomQuests(cExtraServer * sManager, SDHP_GETPLAYERCUSTOMQUESTS_SAVE * lpMsg);
void GDGetPlayerCustomQuests(cExtraServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * lpMsg);
void GJDeletePlayerQ5Info(cExtraServer * sManager, int aIndex, SDHP_DELETEPLAYERS5QUEST_INFO * pRecv);
void GJSetPlayerS5QuestInfo(cExtraServer * sManager, int aIndex, SDHP_GETPLAYERS5QUEST_INFOSAVE * lpMsg);
void GDGetPlayerS5Quests(cExtraServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * lpMsg);
void GDSetOfficialHelper(cExtraServer * sManager, SDHP_GETPLAYEROFFICIALHELPER_SAVE * aRecv);
void GDGetPlayerOfficialHelper(cExtraServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * lpMsg);
void DuelScore(cExtraServer * sManager,PMSG_ANS_DUELSCORE * lpMsg);
void DGScore(cExtraServer * sManager,PMSG_ANS_DGSCORE * lpMsg);
void IGScore(cExtraServer * sManager,PMSG_ANS_IGSCORE * lpMsg);
void MonstersKilledScore(cExtraServer * sManager,PMSG_ANS_MONSTERKILLED * lpMsg);