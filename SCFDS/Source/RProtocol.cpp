#include "stdafx.h"
#include "config.h"
#include "winutil.h"
#include "RProtocol.h"
#include "../SCFExDB_Defines.h"
#include "../stdafx.h"

void DevilSqureScore(cDataServer * sManager, PMSG_ANS_EVENTUSERSCORE * lpMsg)
{
	sManager->vLogAddTD("[DEVIL] SERVER:%d-%d, SQUARE:%d, ACCNT:%s, CHAR:%s, CLASS:%d, SCORE:%d",
		lpMsg->ServerCode/20+1,lpMsg->ServerCode % 20 + 1,lpMsg->SquareNum,lpMsg->AccountID,lpMsg->GameID,lpMsg->Class,lpMsg->Score);

	sManager->db.ExecQuery("EXEC %s.dbo.SP_POINT_ACCUMULATION '%d', '%d', '%s', '%s', '%d', '%d'",
		sManager->m_DBName3,lpMsg->ServerCode/20+1,lpMsg->SquareNum,lpMsg->AccountID,lpMsg->GameID,lpMsg->Class,lpMsg->Score);
	sManager->db.Fetch();
	sManager->db.Close();
}

void EGReqBloodCastleEnterCount(cDataServer * sManager, int aIndex,PMSG_REQ_BLOODCASTLE_ENTERCOUNT * lpMsg)
{
	PMSG_ANS_BLOODCASTLE_ENTERCOUNT pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_RProtocol, 0x0B, sizeof(pMsg));

	memcpy(pMsg.AccountID,lpMsg->AccountID,sizeof(pMsg.AccountID));
	memcpy(pMsg.GameID,lpMsg->GameID,sizeof(pMsg.GameID));

	pMsg.ServerCode = lpMsg->ServerCode;
	pMsg.iObjIndex = lpMsg->iObjIndex;

	if(sManager->db.ExecQuery("EXEC %s.dbo.SP_LEFT_ENTERCOUNT_BC '%s', '%s', '%d'",sManager->m_DBName3,lpMsg->AccountID,lpMsg->GameID,lpMsg->ServerCode) == TRUE)
	{
		sManager->db.Fetch();
		pMsg.iLeftCount = sManager->db.GetAsInteger("LeftEnterCount");
	}
	else
	{
		pMsg.iLeftCount = -1;
	}
	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void BloodCastleScore_5TH(cDataServer * sManager, PMSG_ANS_BLOODCASTLESCORE_5TH * lpMsg)
{
	sManager->vLogAddTD("[BLOOD] SERVER:%d-%d, BRIDGE:%d, ACCNT:%s, CHAR:%s, CLASS:%d, SCORE:%d, LEFTTIME:%d, ALIVEPARTY:%d",
		lpMsg->ServerCode / 20 + 1,lpMsg->ServerCode % 20 + 1,lpMsg->BridgeNum,lpMsg->AccountID,lpMsg->GameID,lpMsg->Class,lpMsg->Score,lpMsg->iLeftTime,lpMsg->iAlivePartyCount);

	sManager->db.ExecQuery("EXEC %s.dbo.SP_POINT_ACCM_BC_5TH '%d', '%d', '%s', '%s', '%d', '%d', '%d', '%d'",
		sManager->m_DBName3,lpMsg->ServerCode / 20 + 1,lpMsg->BridgeNum,lpMsg->AccountID,lpMsg->GameID,lpMsg->Class,lpMsg->Score,lpMsg->iLeftTime,lpMsg->iAlivePartyCount);
	sManager->db.Fetch();
	sManager->db.Close();
}