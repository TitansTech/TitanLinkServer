

#include "ServerManager.h"

/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu RankingServer DevilSquare Score
 *	Direction : GameServer -> RankingServer
 *  Code : 0xC1
 *	HeadCode : 0x01
 */

struct PMSG_ANS_EVENTUSERSCORE
{
	PBMSG_HEAD2 h;	// C1:01
	char AccountID[10];	// 3
	char GameID[10];	// D
	int ServerCode;	// 18
	int Score;	// 1C
	int Class;	// 20
	int SquareNum;	// 24
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu RankingServer Get BC Enter Count
 *	Direction : GameServer -> RankingServer
 *  Code : 0xC1
 *	HeadCode : 0x0B
 */

struct PMSG_REQ_BLOODCASTLE_ENTERCOUNT
{
	PBMSG_HEAD2 h;	// C1:0B
	char AccountID[10];	// 3
	char GameID[10];	// D
	int ServerCode;	// 18
	int iObjIndex;	// 1C
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu RankingServer BloodCastle_5TH Score
 *	Direction : GameServer -> RankingServer
 *  Code : 0xC1
 *	HeadCode : 0x0D
 */

struct PMSG_ANS_BLOODCASTLESCORE_5TH
{
	PBMSG_HEAD2 h;	// C1:0D
	char AccountID[10];	// 3
	char GameID[10];	// D
	int ServerCode;	// 18
	int Score;	// 1C
	int Class;	// 20
	int BridgeNum;	// 24
	int iLeftTime;	// 28
	int iAlivePartyCount;	// 2C
};

struct PMSG_ANS_BLOODCASTLE_ENTERCOUNT {
  PBMSG_HEAD2 h;
  char AccountID[10];
  char GameID[10];
  int ServerCode;
  int iObjIndex;
  int iLeftCount;
};



void DevilSqureScore(cDataServer * sManager, PMSG_ANS_EVENTUSERSCORE * pMsg);
void EGReqBloodCastleEnterCount(cDataServer * sManager, int aIndex,PMSG_REQ_BLOODCASTLE_ENTERCOUNT * lpMsg);
void BloodCastleScore_5TH(cDataServer * sManager, PMSG_ANS_BLOODCASTLESCORE_5TH * lpMsg);