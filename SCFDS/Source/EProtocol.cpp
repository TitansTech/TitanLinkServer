#include "stdafx.h"
#include "config.h"
#include "winutil.h"
#include "EProtocol.h"
#include "../SCFExDB_Defines.h"
#include "../stdafx.h"

void EProtocolCore(cDataServer * sManager, int aIndex, BYTE HeadCode, LPBYTE aRecv)
{
	switch ( HeadCode )
	{
	case 0x01:
		EGAnsEventChipInfo(sManager, aIndex,(PMSG_REQ_VIEW_EC_MN *)aRecv);
		break;
	case 0x02:
		EGAnsRegEventChipInfo(sManager, aIndex,(PMSG_REQ_REGISTER_EVENTCHIP *)aRecv);
		break;
	case 0x04:
		EGAnsResetEventChip(sManager, aIndex,(PMSG_REQ_RESET_EVENTCHIP *)aRecv);
		break;
	case 0x05:
		EGAnsEventStoneInfo(sManager, aIndex,(PMSG_REQ_VIEW_EC_MN *)aRecv);
		break;
	case 0x06:
		EGAnsRegEventStoneInfo(sManager, aIndex,(PMSG_REQ_REGISTER_STONES *)aRecv);
		break;
	case 0x07:
		EGAnsDeleteStones(sManager, aIndex,(PMSG_REQ_DELETE_STONES *)aRecv);
		break;
	case 0x08:
		EGAns2AnivRegSerial(sManager, aIndex,(PMSG_REQ_2ANIV_SERIAL *)aRecv);
		break;
	case 0x09:
		EGAnsResetStoneInfo(sManager, aIndex,(PMSG_REQ_RESET_EVENTCHIP *)aRecv);
		break;
	case 0x10:
		EGAnsRegRingGift(sManager, aIndex,(PMSG_REQ_REG_RINGGIFT *)aRecv);
		break;
	case 0x15:
		EGAnsRegCCOfflineGift(sManager, aIndex,(PMSG_REQ_REG_CC_OFFLINE_GIFT *)aRecv);
		break;
	case 0x16:
		EGAnsRegDLOfflineGift(sManager, aIndex,(PMSG_REQ_REG_DL_OFFLINE_GIFT *)aRecv);
		break;
	case 0x17:
		EGAnsRegHTOfflineGift(sManager, aIndex,(PMSG_REQ_REG_HT_OFFLINE_GIFT *)aRecv);
		break;
	}
}

void EGAnsEventChipInfo(cDataServer * sManager, int aIndex,PMSG_REQ_VIEW_EC_MN * lpMsg)
{
	int test = SQL_NO_DATA;
	PMSG_ANS_VIEW_EC_MN pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x01, sizeof(pMsg));


	pMsg.iINDEX = lpMsg->iINDEX;
	memcpy(pMsg.szUID,lpMsg->szUID,sizeof(pMsg.szUID));
	pMsg.szUID[10] = 0;

	if(sManager->db.ExecQuery("SELECT EventChips, Check_Code, MuttoNumber FROM %s.dbo.T_MU2003_EVENT WHERE AccountID='%s'",sManager->m_DBName4,lpMsg->szUID) == FALSE)
	{
		pMsg.bSUCCESS = FALSE;
	}
	else
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			if(sManager->db.GetAsInteger("Check_Code") != 0)
			{
				pMsg.nEVENT_CHIPS = 0;
			}
			else
			{
				pMsg.nEVENT_CHIPS = (short)sManager->db.GetAsInteger("EventChips");
			}

			pMsg.iMUTO_NUM = sManager->db.GetAsInteger("MuttonNumber");
			pMsg.bSUCCESS = TRUE;
		}
		else
		{
			sManager->db.Close();
			if(sManager->db.ExecQuery("INSERT INTO %s.dbo.T_MU2003_EVENT (AccountID) VALUES ('%s')",sManager->m_DBName4,pMsg.szUID) == FALSE)
			{
				pMsg.bSUCCESS = FALSE;
			}
			else
			{
				pMsg.nEVENT_CHIPS = 0;
				pMsg.iMUTO_NUM = 0;
				pMsg.bSUCCESS = TRUE;
			}
		}
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegEventChipInfo(cDataServer * sManager, int aIndex,PMSG_REQ_REGISTER_EVENTCHIP * lpMsg)
{
	PMSG_ANS_REGISTER_EVENTCHIP pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x02, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.Pos = lpMsg->Pos;

	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;
	pMsg.bSUCCESS = FALSE;

	if(sManager->db.ExecQuery("UPDATE %s.dbo.T_MU2003_EVENT SET EventChips = EventChips + 1 WHERE AccountID = '%s'",sManager->m_DBName4,lpMsg->szUID))
	{
		sManager->db.Fetch();
		sManager->db.Close();

		if(sManager->db.ExecQuery("SELECT EventChips FROM %s.dbo.T_MU2003_EVENT WHERE AccountID='%s'",sManager->m_DBName4,lpMsg->szUID))
		{
			if(sManager->db.Fetch() != SQL_NO_DATA)
			{
				pMsg.nEVENT_CHIPS = (short)sManager->db.GetAsInteger("EventChips");

				if(pMsg.nEVENT_CHIPS != -1)
				{
					pMsg.nEVENT_CHIPS = TRUE;
				}
			}
		}
	}

	sManager->db.Close();

	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsResetEventChip(cDataServer * sManager, int aIndex,PMSG_REQ_RESET_EVENTCHIP * lpMsg)
{
	PMSG_ANS_RESET_EVENTCHIP pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x04, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0;

	if(sManager->db.ExecQuery("UPDATE %s.dbo.T_MU2003_EVENT SET Check_Code = 2 WHERE AccountID = '%s'",sManager->m_DBName4,lpMsg->szUID))
	{
		sManager->db.Fetch();
		pMsg.bSUCCESS = TRUE;
	}
	else
	{
		pMsg.bSUCCESS = FALSE;
	}

	sManager->db.Close();

	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsEventStoneInfo(cDataServer * sManager, int aIndex,PMSG_REQ_VIEW_EC_MN * lpMsg)
{
	PMSG_ANS_VIEW_STONES pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x05, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	if(sManager->db.ExecQuery("SELECT StoneCount, Check_Code FROM %s.dbo.T_BLOOD_CASTLE WHERE AccountID='%s'",sManager->m_DBName4,lpMsg->szUID) == FALSE)
	{
		sManager->db.Fetch();
		pMsg.bSUCCESS = FALSE;
	}
	else
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			if(sManager->db.GetAsInteger("Check_Code") != 0)
			{
				pMsg.iStoneCount = 0;
			}
			else
			{
				pMsg.iStoneCount = sManager->db.GetAsInteger("StoneCount");
			}
			pMsg.bSUCCESS = TRUE;
		}
		else
		{
			sManager->db.Close();

			if(sManager->db.ExecQuery("INSERT INTO %s.dbo.T_BLOOD_CASTLE (AccountID) VALUES ('%s')",sManager->m_DBName4,lpMsg->szUID))
			{
				sManager->db.Fetch();
				pMsg.bSUCCESS = TRUE;
				pMsg.iStoneCount = 0;
			}
		}
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegEventStoneInfo(cDataServer * sManager, int aIndex,PMSG_REQ_REGISTER_STONES * lpMsg)
{
	PMSG_ANS_REGISTER_STONES pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x06, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.iPosition = lpMsg->iPosition;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	if(sManager->db.ExecQuery("UPDATE %s.dbo.T_BLOOD_CASTLE SET StoneCount = StoneCount + 1 WHERE AccountID = '%s'",sManager->m_DBName4,lpMsg->szUID))
	{
		sManager->db.Fetch();
		sManager->db.Close();
		if(sManager->db.ExecQuery("SELECT StoneCount FROM %s.dbo.T_BLOOD_CASTLE WHERE AccountID='%s'",sManager->m_DBName4,lpMsg->szUID))
		{
			if(sManager->db.Fetch() != SQL_NO_DATA)
			{
				pMsg.iStoneCount = sManager->db.GetAsInteger("StoneCount");

				if(pMsg.iStoneCount != -1)
				{
					pMsg.bSUCCESS = TRUE;
				}
			}
			else
			{
				pMsg.bSUCCESS = FALSE;
			}
		}
	}
	else
	{
		pMsg.bSUCCESS = FALSE;
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsDeleteStones(cDataServer * sManager, int aIndex,PMSG_REQ_DELETE_STONES * lpMsg)
{
	PMSG_ANS_DELETE_STONES pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x07, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	pMsg.bSUCCESS = FALSE;

	if(sManager->db.ExecQuery("SELECT StoneCount FROM %s.dbo.T_BLOOD_CASTLE WHERE AccountID='%s'",sManager->m_DBName4,lpMsg->szUID) == FALSE || sManager->db.Fetch() == SQL_NO_DATA)
	{
		sManager->db.Fetch();
		pMsg.bSUCCESS = FALSE;
	}
	else
	{
		int Stones = sManager->db.GetAsInteger("StoneCount");

		sManager->db.Close();

		if(Stones != -1 && Stones >= lpMsg->iStoneCount)
		{
			if(sManager->db.ExecQuery("UPDATE %s.dbo.T_BLOOD_CASTLE SET StoneCount = StoneCount - %d WHERE AccountID='%s'",sManager->m_DBName4,lpMsg->szUID) == FALSE)
			{
		
			}
			else
			{
				sManager->db.Fetch();
				pMsg.bSUCCESS = TRUE;
				pMsg.iStoneCount = Stones - lpMsg->iStoneCount;
			}
		}
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAns2AnivRegSerial(cDataServer * sManager, int aIndex,PMSG_REQ_2ANIV_SERIAL * lpMsg)
{
	PMSG_ANS_2ANIV_SERIAL pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x08, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);

	if(sManager->db.ExecQuery("EXEC %s.dbo.SP_REG_SERIAL '%s', '%d', '%s', '%s', '%s'",sManager->m_DBName4,lpMsg->szUID,lpMsg->iMEMB_GUID,lpMsg->SERIAL1,lpMsg->SERIAL2,lpMsg->SERIAL3) && sManager->db.Fetch() != SQL_NO_DATA)
	{
		sManager->db.Fetch();
		pMsg.btIsRegistered = (BYTE)sManager->db.GetAsInteger("RegResult");
		pMsg.iGiftNumber = sManager->db.GetAsInteger("F_Register_Section");

		if(pMsg.iGiftNumber == -1)
		{
			pMsg.btIsRegistered = 4;
		}
	}
	else
	{
		pMsg.btIsRegistered = 4;
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsResetStoneInfo(cDataServer * sManager, int aIndex,PMSG_REQ_RESET_EVENTCHIP * lpMsg)
{
	PMSG_ANS_RESET_EVENTCHIP pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x09, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0;

	if(sManager->db.ExecQuery("UPDATE %s.dbo.T_BLOOD_CASTLE SET Check_Code = 2 WHERE AccountID = '%s'",sManager->m_DBName4,lpMsg->szUID) == FALSE)
	{
		pMsg.bSUCCESS = FALSE;
	}
	else
	{
		sManager->db.Fetch();
		pMsg.bSUCCESS = TRUE;
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegRingGift(cDataServer * sManager, int aIndex,PMSG_REQ_REG_RINGGIFT * lpMsg)
{
	PMSG_ANS_REG_RINGGIFT pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x10, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x10;

	pMsg.btIsRegistered = 0;

	if(sManager->db.ExecQuery("EXEC %s.dbo.SP_REG_RINGGIFT '%s', '%d'",sManager->m_DBName4,lpMsg->szUID,lpMsg->btGiftKind) == FALSE || sManager->db.Fetch() == SQL_NO_DATA)
	{

	}
	else
	{
		sManager->db.Fetch();
		pMsg.btIsRegistered = (BYTE)sManager->db.GetAsInteger("RegResult");
		pMsg.btGiftKind = (BYTE)sManager->db.GetAsInteger("GiftKind");
		pMsg.btGiftSection = lpMsg->btGiftKind;

		if(pMsg.btIsRegistered == -1)
		{
			pMsg.btIsRegistered = 0;
		}
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegCCOfflineGift(cDataServer * sManager, int aIndex,PMSG_REQ_REG_CC_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_CC_OFFLINE_GIFT pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x15, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID,lpMsg->szUID);
	strcpy(pMsg.szNAME,lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;
	
	pMsg.iResultCode = 0;

	if(sManager->db.ExecQuery("EXEC %s.dbo.SP_REG_CC_OFFLINE_GIFT '%s', '%s', %d",sManager->m_DBName4,lpMsg->szUID,lpMsg->szNAME,lpMsg->wServerCode) == FALSE || sManager->db.Fetch() == SQL_NO_DATA)
	{

	}
	else
	{
		sManager->db.Fetch();
		pMsg.iResultCode = sManager->db.GetAsInteger("ResultCode");
		if(pMsg.iResultCode != -1)
		{
			sManager->db.GetAsString("GiftName",pMsg.szGIFT_NAME,sizeof(pMsg.szGIFT_NAME) - 1);
		}
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegDLOfflineGift(cDataServer * sManager, int aIndex,PMSG_REQ_REG_DL_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_DL_OFFLINE_GIFT pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x16, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID,lpMsg->szUID);
	strcpy(pMsg.szNAME,lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;
	
	pMsg.iResultCode = 0;

	if(sManager->db.ExecQuery("EXEC %s.dbo.SP_REG_DL_OFFLINE_GIFT '%s', '%s', %d",sManager->m_DBName4,lpMsg->szUID,lpMsg->szNAME,lpMsg->wServerCode) == FALSE || sManager->db.Fetch() == SQL_NO_DATA)
	{

	}
	else
	{
		sManager->db.Fetch();
		pMsg.iResultCode = sManager->db.GetAsInteger("ResultCode");
		if(pMsg.iResultCode != -1)
		{
			sManager->db.GetAsString("GiftName",pMsg.szGIFT_NAME,sizeof(pMsg.szGIFT_NAME));
		}
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegHTOfflineGift(cDataServer * sManager, int aIndex,PMSG_REQ_REG_HT_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_HT_OFFLINE_GIFT pMsg = {0};

	PHeadSubSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_EProtocol, 0x17, sizeof(pMsg));

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID,lpMsg->szUID);
	strcpy(pMsg.szNAME,lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;
	
	pMsg.iResultCode = 0;

	if(sManager->db.ExecQuery("EXEC %s.dbo.SP_REG_HT_OFFLINE_GIFT '%s', '%s', %d",sManager->m_DBName4,lpMsg->szUID,lpMsg->szNAME,lpMsg->wServerCode) == FALSE || sManager->db.Fetch() == SQL_NO_DATA)
	{

	}
	else
	{
		pMsg.iResultCode = sManager->db.GetAsInteger("ResultCode");
		if(pMsg.iResultCode != -1)
		{
			sManager->db.GetAsString("GiftName",pMsg.szGIFT_NAME,sizeof(pMsg.szGIFT_NAME));
		}
	}

	sManager->db.Close();
	sManager->DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}
