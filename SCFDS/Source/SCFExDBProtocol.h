
#pragma once

#include "ServerManager.h"

#define MAX_SKILLTREE					60

#define MAX_BOTRACER_REWARD				10

#define MAX_TITANRANK_REW				3

#define MAX_TITANLOTTERY_NUMS			4

struct MASTERSKILLS
{
	WORD Type;
	BYTE Level;
};

struct SDHP_GETPLAYERMASTERSKILL_SAVE
{
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short aIndex;
	MASTERSKILLS Skill[MAX_SKILLTREE];
};

typedef struct GAMESERVER_OBJECT {
	short	m_aIndex;
	char	m_sAccountID[11];
} *PGAMESERVER_OBJECT;


struct SDHP_SETSTATUSBAN_INFOSAVE
{
	PBMSG_HEAD h;
	char Name[11];
	BYTE Ban;
	BOOL Type;
};


struct SDHP_SETMARRYDIVORSE_INFOSAVE
{
	PBMSG_HEAD h;
	char Name[11];
};


struct SDHP_GETACCOUNTEXTRAINFO_SAVE
{
	PBMSG_HEAD h;
	char AccountID[11];
	short aIndex;
	BYTE Vip;
	int VipMoney;
	int VipDays;
	int WarehouseVipCount;
	BYTE HaveBot;
	int LuckyCoins;
	BYTE ExWarehouse;
	BYTE LotteryCnt;
};

struct SDHP_GETACCOUNTEXTRAINFO
{
	PBMSG_HEAD h;	// C1:01
	char AccountID[11];
	short aIndex;
};

struct SDHP_GETPLAYERINFO
{
	PBMSG_HEAD h;	// C1:01
	char AccountID[11];
	char Name[11];
	short aIndex;
};

struct SDHP_GETPLAYERGENSINFO_SAVE
{
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short aIndex;
};

struct botReward_Data_Struct
{
	WORD num;
	BYTE Level;
	BYTE Opt;
	BYTE Luck;
	BYTE Skill;
	BYTE Dur;
	BYTE Exc;
	BYTE Anc;

	BYTE Sock1;
	BYTE Sock2;
	BYTE Sock3;
	BYTE Sock4;
	BYTE Sock5;

	BYTE Days;
	BYTE JOH;
	BYTE IsForeverFFFE;

	
	int Zen;
	int VipMoney;
};

struct SDHP_GETREWARD_INFOSAVE
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short aIndex;

	int ID_Num;
	botReward_Data_Struct rew;
};

struct SDHP_GETREWARD_INFODEL
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	int ID_Num;
};

struct SDHP_GETPLAYEREXTRAINFO
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short aIndex;
};

struct SDHP_GENSGUILD_INFO_REQUEST
{
	PBMSG_HEAD h;	// C1:7E
	char GuildName[9];	// 8
};

struct SDHP_GENSGUILD_INFO_GET
{
	PBMSG_HEAD h;	// C1:7E
	char GuildName[9];	// 8
	BYTE Family;
};

struct SDHP_SETPLAYERGENSLEAVEINFO_SAVE
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
};

struct SDHP_GETPLAYEREXTRAINFO_SAVE
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short aIndex;
	int MasterLevel;
	int MasterPoints;
	int Resets;
	int PCPoints;
	WORD CShop_SealItem;
	int CShop_SealTime;
	WORD CShop_ScrollItem;
	int CShop_ScrollTime;
	BOOL Married;
	char MarriedName[11];

	BYTE Family;
	int Contribution;
	int Rank;
	BYTE NotAllow;
	BYTE RecvRewItem;
};

struct SDHP_GETPLAYERINFO_INFOSAVE
{
	PBMSG_HEAD h;
	char Name[11];
	int MasterLevel;
	int MasterPoints;
	int Resets;
	int PCPoints;
	WORD CShop_SealItem;
	int CShop_SealTime;
	WORD CShop_ScrollItem;
	int CShop_ScrollTime;
	BOOL Married;
	char MarriedName[11];
	BYTE GensFamily;
	int GensCont;
	BYTE GensNotAllow;
	BYTE GensRecvRewItem;
};

struct SDHP_GETWAREHOUSEDB_SAVE_SCFEXDB
{
	PWMSG_HEAD h;
	char AccountID[10];	// 4
	short aIndex;	// E
	int Money;	// 10
	BYTE dbItems[1920];	// 14
	BYTE DbVersion;	// 794
	short pw;	// 796
	int ExNum;
};

struct SDHP_GETWAREHOUSEDB_RESULT_SCFEXDB
{
	PBMSG_HEAD h;
	char AccountID[10];	// 3
	short aIndex;	// E
	int ExNum;
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Get Warehouse Info Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x08
 */
struct SDHP_GETWAREHOUSEDB_SCFEXDB
{
	PBMSG_HEAD h;	// C1:08
	char AccountID[10];	// 3
	short aIndex;	// E
	int ExNum;
	BYTE IsS6E2;
};
struct SDHP_GETWAREHOUSEDB_S6E2_SAVE_SCFEXDB
{
	PWMSG_HEAD h;
	char AccountID[10];	// 4
	short aIndex;	// E
	int Money;	// 10
	BYTE dbItems[3840];	// 14
	BYTE DbVersion;	// 794
	short pw;	// 796
	int ExNum;
};


struct SDHP_WAREHOUSEMONEYEX_SAVE
{
	PBMSG_HEAD h;	// C1:12
	char AccountID[10];	// 3
	short aIndex;	// E
	int Money;	// 10
	int ExNum;
};



struct SDHP_GETPETBOT
{
	PWMSG_HEAD h;	// C1:16
	BYTE Result;
	short aIndex;
	char AccountID[11];
	char Name[11];
	BYTE Class;
	short Level;
	unsigned long Exp;
	int Power;
	int Defense;
	WORD Life;
	WORD MaxLife;
	WORD Mana;
	WORD MaxMana;
	BYTE dbInventory[1728];
	BYTE dbMagicList[180];
	BYTE MapNumber;
	BYTE MapX;
	BYTE MapY;
	int BotLife;
	int BotMaxLife;

	int BotLvlUpDefense;
	int BotLvlUpPower;
	int BotLvlUpLife;
	int BotLvlUpMana;
	int BotLvlUpExp;
	int BotLvlUpMaxLevel;
	BYTE BotVersion;

};
struct SDHP_PETBOTCREATERESULT
{
	PBMSG_HEAD h;	//
	int aIndex;
	char Name[11];
	int BotPetNum;
	BYTE result;
};


struct SDHP_GETHUNTZONEOPEN
{
	PBMSG_HEAD h;
	BYTE subcode;
	int mapServer;
};


struct SDHP_SETHUNTZONEOPEN
{
	PBMSG_HEAD h;
	BYTE subcode;
	int state;
	int mapServer;
};

//-----------------

struct SDHP_GETGUILDWAREHOUSEDB_S6E2_SAVE
{
	PWMSG_HEAD h;
	char G_Name[10];
	char AccountID[10];	// 4
	short aIndex;	// E
	int Money;	// 10
	BYTE dbItems[3840];	// 14
	BYTE DbVersion;	// 794
	short pw;	// 796
};

struct SDHP_GETGUILDWAREHOUSEDB_SAVE
{
	PWMSG_HEAD h;
	char G_Name[10];
	char AccountID[10];	// 4
	short aIndex;	// E
	int Money;	// 10
	BYTE dbItems[1920];	// 14
	BYTE DbVersion;	// 794
	short pw;	// 796
};

struct SDHP_GETGUILDWAREHOUSEDB
{
	PBMSG_HEAD h;	// C1:08
	char G_Name[10];
	char AccountID[10];	// 3
	short aIndex;	// E
	BYTE IsS6E2;
};

struct SDHP_GETWAREHOUSEDB_S6E2_SAVE_EX
{
	PWMSG_HEAD h;
	char AccountID[10];	// 4
	short aIndex;	// E
	int Money;	// 10
	BYTE dbItems[3840];	// 14
	BYTE DbVersion;	// 794
	short pw;	// 796
	int ExNum;
};

struct SDHP_SETPLAYERRACE_INFOSAVE
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	int Seconds;
};

struct SDHP_SETRACEREWARD_INFOSAVE
{
	PBMSG_HEAD h;
	BYTE Count;
	botReward_Data_Struct rew[MAX_BOTRACER_REWARD];
};


struct SDHP_SETRESET_LOG
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	int Reset;
	BYTE Type;
};


struct cObjTitanRank_Info
{
	BYTE Enabled;
	BYTE rIndex;
	BYTE Type;
	BYTE SubType;
	BYTE Class;
	BYTE Top;

	BYTE RewCnt;

	botReward_Data_Struct	Reward[MAX_TITANRANK_REW];
};

struct SDHP_GETRANKINFO
{
	PBMSG_HEAD h;
	BYTE rIndex;
	cObjTitanRank_Info info;
};

struct cObjTitanRank_AccountPlayerINFO
{
	char AccountID[11];
	char Name[11];
};

struct SDHP_GETRANK_INFORECV
{
	PBMSG_HEAD h;
	BYTE rIndex;
	cObjTitanRank_AccountPlayerINFO info[MAX_TITANRANK_REW];
};

struct cObjTitanLottery_LotteryInfo
{
	BYTE	Type;
	int		MinReward;
	int		MaxReward;
	BYTE	AdditionalReward;
};

struct SDHP_SENDLOTTERYINFO
{
	PBMSG_HEAD h;
	BYTE SubType;						//0

	BYTE Nums[MAX_TITANLOTTERY_NUMS];

	cObjTitanLottery_LotteryInfo info;
	botReward_Data_Struct ExReward;
};

struct SDHP_SENDLOTTERYBUYTICKET
{
	PBMSG_HEAD h;
	BYTE SubType;						//1
	
	char AccountID[11];
	char Name[11];
	BYTE Nums[MAX_TITANLOTTERY_NUMS];
	int  ToReward;
};

struct SDHP_RECVLOTTERYINFO
{
	PBMSG_HEAD h;
	
	int WinnersCount;
	int Reward;
	int NextReward;
};

void GJSetPlayerExtraInfo(cDataServer * sManager, int aIndex, SDHP_GETPLAYERINFO_INFOSAVE * lpMsg);
void GJSetStatusBan(cDataServer * sManager, int aIndex, SDHP_SETSTATUSBAN_INFOSAVE * lpMsg);
void GJSetPlayerMarryForceDivorse(cDataServer * sManager, int aIndex, SDHP_SETMARRYDIVORSE_INFOSAVE * lpMsg);
void GJSetAccountExtraInfo(cDataServer * sManager, int aIndex, SDHP_GETACCOUNTEXTRAINFO_SAVE * lpMsg);
void DGGetWarehouseListEx(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB_SCFEXDB * aRecv);
void GDSetWarehouseMoneyEx(cDataServer * sManager, int aIndex, SDHP_WAREHOUSEMONEYEX_SAVE * aRecv);
void GDSetWarehouseListEx(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB_SAVE_SCFEXDB * aRecv);
void GDSetWarehouseList_S6E2(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB_S6E2_SAVE_SCFEXDB * aRecv);
void DGGetPetBot(cDataServer * sManager, int aIndex, SDHP_GETACCOUNTEXTRAINFO * aRecv);
void DGSetPetBot(cDataServer * sManager, int aIndex, SDHP_GETPETBOT * aRecv);
void GDCreateResultPetBot(cDataServer * sManager, int aIndex, SDHP_PETBOTCREATERESULT * pRecv);
void GensGuildFamily(cDataServer * sManager, int aIndex, SDHP_GENSGUILD_INFO_REQUEST * lpMsg);
void GensLeaveFamily(cDataServer * sManager, int aIndex, SDHP_SETPLAYERGENSLEAVEINFO_SAVE * lpMsg);
void GS_DGAnsHuntZoneOpen(cDataServer * sManager, int aIndex, SDHP_GETHUNTZONEOPEN * lpMsg);
void GS_DGGetHuntZoneOpen(cDataServer * sManager, int aIndex, SDHP_SETHUNTZONEOPEN * lpMsg);
void GensRankingUpdate(cDataServer * sManager);
void ClearCoins(cDataServer * sManager);
void GDSetGuildWarehouseList(cDataServer * sManager, int aIndex, SDHP_GETGUILDWAREHOUSEDB_SAVE * aRecv);
void GDSetGuildWarehouseList_S6E2(cDataServer * sManager, int aIndex, SDHP_GETGUILDWAREHOUSEDB_S6E2_SAVE * aRecv);
void GDDelGuildWarehouseList(cDataServer * sManager, int aIndex, SDHP_GETGUILDWAREHOUSEDB * aRecv);
void DGGetGuildWarehouseList(cDataServer * sManager, int aIndex, SDHP_GETGUILDWAREHOUSEDB * aRecv);

void GDGetPlayerExtraInfo(cDataServer * sManager, int aIndex, int pIndex, char * Name, char * AccountID);
void GJSetPlayerMasterSkillInfo(cDataServer * sManager, int aIndex, SDHP_GETPLAYERMASTERSKILL_SAVE * lpMsg);
//void GDGetPlayerMasterSkillInfo(cDataServer * sManager, int aIndex, int pIndex, char * Name, char * AccountID);
void GDGetPlayerMasterSkillInfo2(cDataServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * aRecv);

void GetAccountExtraInfo(cDataServer * sManager, int aIndex, SDHP_GETACCOUNTEXTRAINFO * aRecv);
void GDGetAccountExtraInfo(cDataServer * sManager, int aIndex, int pIndex, char * AccountID);



void GJSetPlayerRaceInfo(cDataServer * sManager, int aIndex, SDHP_SETPLAYERRACE_INFOSAVE * lpMsg);
void GJSetRaceRewardInfo(cDataServer * sManager, SDHP_SETRACEREWARD_INFOSAVE * lpMsg);


void DelTitanReward(cDataServer * sManager, SDHP_GETREWARD_INFODEL * aRecv);
void GetTitanReward(cDataServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * aRecv);

void SetResetLog(cDataServer * sManager, SDHP_SETRESET_LOG * aRecv);


void GetTitanRank(cDataServer * sManager, SDHP_GETRANKINFO * aRecv);

void SetTitanLotteryInfo(cDataServer * sManager, SDHP_SENDLOTTERYINFO * aRecv);
void SetTitanLotteryPlayerBuyTicket(cDataServer * sManager, SDHP_SENDLOTTERYBUYTICKET * aRecv);
void GetRewardInfo(cDataServer * sManager, SDHP_GETREWARD_INFOSAVE * lpMsg);