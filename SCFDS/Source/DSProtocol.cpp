#include "stdafx.h"
#include "config.h"
#include "winutil.h"
#include "DSProtocol.h"
#include "../stdafx.h"
#include "Log.h"
#include "SCFExDBProtocol.h"
#include "RProtocol.h"
#include "EProtocol.h"
#include "../SCFExDB_Defines.h"
#include "ExServerProtocol.h"
//#include "ServerManager.h"


void DSProtocolCore(cDataServer * sManager, int aIndex, BYTE Head, LPBYTE aRecv, int aLen)
{
	switch (Head)
	{
		case 0x90:
			DataServerLogin(sManager, aIndex, (SDHP_SERVERINFO *)aRecv);
			break;

		case 0x91:
			JGPGetCharList(sManager, aIndex, (SDHP_GETCHARLIST *)aRecv);
			break;

		case 0x94:
			JGCharacterCreateRequest(sManager, aIndex, (SDHP_CREATECHAR *)aRecv);
			break;

		case 0x95:
			JGCharDelRequest(sManager, aIndex, (SDHP_CHARDELETE *)aRecv);
			break;

		case 0x06:
			{				
				if(g_Config.IsS6E2 == 0)
					JGGetCharacterInfo(sManager, aIndex, (SDHP_DBCHARINFOREQUEST *)aRecv);
				else
					JGGetCharacterInfoS6E2(sManager, aIndex, (SDHP_DBCHARINFOREQUEST *)aRecv);
			}break;

		case 0x07:
			{
				if(g_Config.IsS6E2 == 0)
					GJSetCharacterInfo(sManager, aIndex, (SDHP_DBCHAR_INFOSAVE *)aRecv);
				else
					GJSetCharacterInfoS6E2(sManager, aIndex, (SDHP_DBCHAR_INFOSAVES6E2 *)aRecv);
			}break;

		case 0x08:
			{
				if(g_Config.IsS6E2 == 0)
					DGGetWarehouseList(sManager, aIndex, (SDHP_GETWAREHOUSEDB *)aRecv);
				else
					DGGetWarehouseListS6E2(sManager, aIndex, (SDHP_GETWAREHOUSEDB *)aRecv);
			}
			break;		

		case 0x09:
			{
				if(g_Config.IsS6E2 == 0)
					GDSetWarehouseList(sManager, aIndex, (SDHP_GETWAREHOUSEDB_SAVE *)aRecv);
				else
					GDSetWarehouseListS6E2(sManager, aIndex, (SDHP_GETWAREHOUSEDB_S6E2_SAVE *)aRecv);
			}
			break;

		case 0x11:
			GDUserItemSave(sManager, aIndex, (SDHP_DBCHAR_ITEMSAVE *)aRecv);
			break;

		case 0x12:
			GDSetWarehouseMoney(sManager, aIndex, (SDHP_WAREHOUSEMONEY_SAVE *)aRecv);
			break;

		case 0x52:
			ItemSerialCreateRecv(sManager, aIndex, (SDHP_ITEMCREATE *)aRecv);
			break;

		case 0x55:
			PetItemSerialCreateRecv(sManager, aIndex, (SDHP_ITEMCREATE *)aRecv);
			break;

		case 0x56:
			DGRecvPetItemInfo(sManager, aIndex, (SDHP_REQUEST_PETITEM_INFO *)aRecv);
			break;

		case 0x57:
			GDSavePetItemInfo(sManager, aIndex, (SDHP_SAVE_PETITEM_INFO *)aRecv);
			break;

		case 0x60:
			DGOptionDataRecv(sManager, aIndex, (SDHP_SKILLKEYDATA *)aRecv);
			break;

		case 0x80:
		{
			PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;
			// ----
			switch(lpDef1->subcode)
			{
			case 0x01:
				GS_DGAnsOwnerGuildMaster(sManager, aIndex, (CSP_REQ_OWNERGUILDMASTER*)aRecv);
				break;
			case 0x03:
				GS_DGAnsCastleNpcBuy(sManager, aIndex, (CSP_REQ_NPCBUY *)aRecv);
				break;
			case 0x04:
				GS_DGAnsCastleNpcRepair(sManager, aIndex, (CSP_REQ_NPCREPAIR*)aRecv);
				break;
			case 0x05:
				GS_DGAnsCastleNpcUpgrade(sManager, aIndex, (CSP_REQ_NPCUPGRADE*)aRecv);
				break;
			case 0x06:
				GS_DGAnsTaxInfo(sManager, aIndex, (CSP_REQ_TAXINFO*)aRecv);
				break;
			case 0x07:
				GS_DGAnsTaxRateChange(sManager, aIndex, (CSP_REQ_TAXRATECHANGE*)aRecv);
				break;
			case 0x08:
				GS_DGAnsCastleMoneyChange(sManager, aIndex, (CSP_REQ_MONEYCHANGE*)aRecv);
				break;
			case 0x09:
				GS_DGAnsSiegeDateChange(sManager, aIndex, (CSP_REQ_SDEDCHANGE*)aRecv);
				break;
			case 0x0A:
				GS_DGAnsGuildMarkRegInfo(sManager, aIndex, (CSP_REQ_GUILDREGINFO*)aRecv);
				break;
			case 0x0B:
				GS_DGAnsSiegeEndedChange(sManager, aIndex, (CSP_REQ_SIEGEENDCHANGE*)aRecv);
				break;
			case 0x0C:
				GS_DGAnsCastleOwnerChange(sManager, aIndex, (CSP_REQ_CASTLEOWNERCHANGE*)aRecv);
				break;
			case 0x0D:
				GS_DGAnsRegAttackGuild(sManager, aIndex, (CSP_REQ_REGATTACKGUILD*)aRecv);
				break;
			case 0x0E:
				GS_DGAnsRestartCastleState(sManager, aIndex, (CSP_REQ_CASTLESIEGEEND*)aRecv);
				break;
			case 0x0F:
				GS_DGAnsMapSvrMsgMultiCast(sManager, aIndex, (CSP_REQ_MAPSVRMULTICAST*)aRecv);
				break;
			case 0x10:
				GS_DGAnsRegGuildMark(sManager, aIndex, (CSP_REQ_GUILDREGMARK*)aRecv);
				break;
			case 0x11:
				GS_DGAnsGuildMarkReset(sManager, aIndex, (CSP_REQ_GUILDRESETMARK*)aRecv);
				break;
			case 0x12:
				GS_DGAnsGuildSetGiveUp(sManager, aIndex, (CSP_REQ_GUILDSETGIVEUP*)aRecv);
				break;
			case 0x16:
				GS_DGAnsNpcRemove(sManager, aIndex, (CSP_REQ_NPCREMOVE*)aRecv);
				break;
			case 0x17:
				GS_DGAnsCastleStateSync(sManager, aIndex, (CSP_REQ_CASTLESTATESYNC*)aRecv);
				break;
			case 0x18:
				GS_DGAnsCastleTributeMoney(sManager, aIndex, (CSP_REQ_CASTLETRIBUTEMONEY *)aRecv);
				break;
			case 0x19:
				GS_DGAnsResetCastleTaxInfo(sManager, aIndex, (CSP_REQ_RESETCASTLETAXINFO*)aRecv);
				break;
			case 0x1A:
				GS_DGAnsResetSiegeGuildInfo(sManager, aIndex, (CSP_REQ_RESETSIEGEGUILDINFO*)aRecv);
				break;
			case 0x1B:
				GS_DGAnsResetRegSiegeInfo(sManager, aIndex, (CSP_REQ_RESETREGSIEGEINFO *)aRecv);
				break;
			}
		}
		break;

		case 0x81:
			GS_DGAnsCastleInitData(sManager, aIndex, (CSP_REQ_CSINITDATA *)aRecv);
			break;

		case 0x83:
			GS_DGAnsAllGuildMarkRegInfo(sManager, aIndex, (CSP_REQ_ALLGUILDREGINFO*)aRecv);
			break;

		case 0x84:
			GS_DGAnsFirstCreateNPC(sManager, aIndex, (CSP_REQ_NPCSAVEDATA*)aRecv);
			break;

		case 0x85:
			GS_DGAnsCalcRegGuildList(sManager, aIndex, (CSP_REQ_CALCREGGUILDLIST*)aRecv);
			break;

		case 0x86:
			GS_DGAnsCsGulidUnionInfo(sManager, aIndex, (CSP_REQ_CSGUILDUNIONINFO *)aRecv);
			break;

		case 0x87:
			GS_DGAnsCsSaveTotalGuildInfo(sManager, aIndex, (CSP_REQ_CSSAVETOTALGUILDINFO*)aRecv);
			break;

		case 0x88:
			GS_DGAnsCsLoadTotalGuildInfo(sManager, aIndex, (CSP_REQ_CSLOADTOTALGUILDINFO*)aRecv);
			break;

		case 0x89:
			GS_DGAnsCastleNpcUpdate(sManager, aIndex, (CSP_REQ_NPCUPDATEDATA*)aRecv);
			break;

		case 0xB0:
			DGAnsCrywolfSync(sManager, aIndex, (CWP_REQ_CRYWOLFSYNC*)aRecv);
			break;

		case 0xB1:
			DGAnsCrywolfInfoLoad(sManager, aIndex, (CWP_REQ_CRYWOLFINFOLOAD*)aRecv);
			break;

		case 0xCF:
			DGMoveOtherServer(sManager, aIndex, (SDHP_CHARACTER_TRANSFER *)aRecv);
			break;

		/*
			SCFExDB Mix Protocol
		*/
			
		//Ranking Server Protocol
		case SCFExDB_GSSend_RProtocol://Ranking Server
		{
			switch(aRecv[3])//SubCode
			{
				case 0x01:
					DevilSqureScore(sManager, (PMSG_ANS_EVENTUSERSCORE *)aRecv);
					break;
				case 0x0B:
					EGReqBloodCastleEnterCount(sManager, aIndex,(PMSG_REQ_BLOODCASTLE_ENTERCOUNT *)aRecv);
					break;
				case 0x0D:
					BloodCastleScore_5TH(sManager, (PMSG_ANS_BLOODCASTLESCORE_5TH *)aRecv);
					break;
			}
		}break;

		//EventServer Protocol
		case SCFExDB_GSSend_EProtocol:
		{
			EProtocolCore(sManager, aIndex,aRecv[3],aRecv);
		}break;
		
		//SCFExDB Protocol
		
		case SCFExDB_GSSend_SiegeProtocol://SIEGE THING
			{
				switch(aRecv[3])
				{
					case 0x00:
					{
						GS_DGAnsHuntZoneOpen(sManager, aIndex, (SDHP_GETHUNTZONEOPEN *) aRecv);
					}break;
					case 0x01:
					{
						GS_DGGetHuntZoneOpen(sManager, aIndex, (SDHP_SETHUNTZONEOPEN *) aRecv);
					}break;
				}
			}break;
		//--
		case SCFExDB_GSSend_GetWarehouseListEx:
			DGGetWarehouseListEx(sManager, aIndex, (SDHP_GETWAREHOUSEDB_SCFEXDB *) aRecv);
			break;
		case SCFExDB_GSSend_SetWarehouseListEx:
			GDSetWarehouseListEx(sManager, aIndex, (SDHP_GETWAREHOUSEDB_SAVE_SCFEXDB *) aRecv);
			break;
		case SCFExDB_GSSend_SetWarehouseListExS6E2:
			GDSetWarehouseList_S6E2(sManager, aIndex, (SDHP_GETWAREHOUSEDB_S6E2_SAVE_SCFEXDB *) aRecv);
			break;
		case SCFExDB_GSSend_SetWarehouseMoneyEx:
			GDSetWarehouseMoneyEx(sManager, aIndex, (SDHP_WAREHOUSEMONEYEX_SAVE *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_AskPetBot:
			DGGetPetBot(sManager, aIndex, (SDHP_GETACCOUNTEXTRAINFO *) aRecv);
			break;
		case SCFExDB_GSSend_SetPetBot:
			DGSetPetBot(sManager, aIndex, (SDHP_GETPETBOT *) aRecv);
			break;
		case SCFExDB_GSSend_CreatePetBot:
			GDCreateResultPetBot(sManager, aIndex, (SDHP_PETBOTCREATERESULT *) aRecv);
			break;		
		//--
		case SCFExDB_GSSend_DelGuildWarehouseList:
			GDDelGuildWarehouseList(sManager, aIndex, (SDHP_GETGUILDWAREHOUSEDB *) aRecv);
			break;
		case SCFExDB_GSSend_SetGuildWarehouseList:
			GDSetGuildWarehouseList(sManager, aIndex, (SDHP_GETGUILDWAREHOUSEDB_SAVE *) aRecv);
			break;
		case SCFExDB_GSSend_SetGuildWarehouseListS6E2:
			GDSetGuildWarehouseList_S6E2(sManager, aIndex, (SDHP_GETGUILDWAREHOUSEDB_S6E2_SAVE *) aRecv);
			break;
		case SCFExDB_GSSend_GetGuildWarehouseList:
			DGGetGuildWarehouseList(sManager, aIndex, (SDHP_GETGUILDWAREHOUSEDB *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_GuildFamily:
			GensGuildFamily(sManager, aIndex, (SDHP_GENSGUILD_INFO_REQUEST *) aRecv);
			break;
		case SCFExDB_GSSend_GensLeaveFamily:
			GensLeaveFamily(sManager, aIndex, (SDHP_SETPLAYERGENSLEAVEINFO_SAVE *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_SetStatusBan:
			GJSetStatusBan(sManager, aIndex, (SDHP_SETSTATUSBAN_INFOSAVE *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_SetPlayerMarryInfo:
			GJSetPlayerMarryForceDivorse(sManager, aIndex, (SDHP_SETMARRYDIVORSE_INFOSAVE *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_SetPlayerExtraInfo:
			GJSetPlayerExtraInfo(sManager, aIndex, (SDHP_GETPLAYERINFO_INFOSAVE *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_SetAccountExtraInfo:
			GJSetAccountExtraInfo(sManager, aIndex, (SDHP_GETACCOUNTEXTRAINFO_SAVE *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_SetPlayerSkillTreeS6E2:
			GJSetPlayerMasterSkillInfo(sManager, aIndex, (SDHP_GETPLAYERMASTERSKILL_SAVE *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_GetAccounExtraInfo:
			GetAccountExtraInfo(sManager, aIndex, (SDHP_GETACCOUNTEXTRAINFO *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_RaceInsert:
			GJSetPlayerRaceInfo(sManager, aIndex, (SDHP_SETPLAYERRACE_INFOSAVE *)aRecv);
			break;
		case SCFExDB_GSSend_RaceReward:
			GJSetRaceRewardInfo(sManager, (SDHP_SETRACEREWARD_INFOSAVE *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_GetBotReward:
			GetTitanReward(sManager, aIndex, (SDHP_GETPLAYEREXTRAINFO *)aRecv);
			break;
		case SCFExDB_GSSend_DelBotReward:
			DelTitanReward(sManager, (SDHP_GETREWARD_INFODEL *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_GetMasterSkillTreeData:
			GDGetPlayerMasterSkillInfo2(sManager, aIndex, (SDHP_GETPLAYEREXTRAINFO *)aRecv);
			break;
		case SCFExDB_GSSend_SetResetLog:
			SetResetLog(sManager, (SDHP_SETRESET_LOG *)aRecv);
			break;
		//--
		case SCFExDB_GSSend_SetTitanRankInfo:
			GetTitanRank(sManager,(SDHP_GETRANKINFO *) aRecv);
			break;
		//--
		case SCFExDB_GSSend_SendTitanLotteryInfo:
			{
				switch(aRecv[3])
				{
					case 0x00:
					{
						SetTitanLotteryInfo(sManager,(SDHP_SENDLOTTERYINFO *) aRecv);
					}break;
					case 0x01:
					{
						SetTitanLotteryPlayerBuyTicket(sManager,(SDHP_SENDLOTTERYBUYTICKET *) aRecv);
					}break;
				}
			}
			break;
		//--
		case SCFExDB_GSSend_RewardInfo:
			GetRewardInfo(sManager, (SDHP_GETREWARD_INFOSAVE *)aRecv);
			break;
	}
}

void DataServerLogin(cDataServer * sManager, int aIndex, SDHP_SERVERINFO * lpMsg)
{
	SDHP_RESULT pResult = {0};
	// ----
	pResult.h.set(0x90, sizeof(pResult));
	pResult.Result = 1;
	// ----
	if(sManager->db.ExecQuery("SELECT ItemCount FROM GameServerInfo WHERE Number=0") == TRUE)
	{
		sManager->db.Fetch();
		// ----
		pResult.ItemCount = sManager->db.GetAsInteger("ItemCount");
	}
	sManager->db.Close();
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void JGPGetCharList(cDataServer * sManager, int aIndex, SDHP_GETCHARLIST * aRecv)
{
	//OutputDebugString("JGPGetCharList\n");

	char cBUFFER[10000]		= {0};
	char szAccountID[11]	= {0};
	TCHAR CharName[5][11]	= {0};
	int iCharCount			= 0;
	int lOfs				= sizeof(SDHP_CHARLISTCOUNT);
	// ----
	BYTE btInvetory[INVENTORY_BINARY_SIZE];
	// ----
	SDHP_CHARLISTCOUNT * pCount = (SDHP_CHARLISTCOUNT *)(cBUFFER);
	SDHP_CHARLIST * pCL = (SDHP_CHARLIST *)(cBUFFER + sizeof(SDHP_CHARLISTCOUNT));
	// ----
	//OutputDebugString("memcpy(szAccountID, aRecv->Id, 10)\n");
	memcpy(szAccountID, aRecv->Id, 10);
	// ----
	pCount->Number = aRecv->Number;
	// ----
	lstrcpy(pCount->AccountId, szAccountID);
	// ----
	sManager->db.ExecQuery("SELECT Id FROM AccountCharacter WHERE Id='%s'", szAccountID);
	// ----
	if(sManager->db.Fetch() == SQL_NO_DATA)
	{
		sManager->db.Close();

		sManager->db.ExecQuery("INSERT INTO AccountCharacter (Id) VALUES ('%s')", szAccountID);
		sManager->db.Fetch();
	}
	// ----
	sManager->db.Close();
	// ----
	sManager->db.ExecQuery("SELECT * FROM AccountCharacter WHERE Id='%s'", szAccountID);
	// ----
	sManager->db.Fetch();
	// ----
	pCount->MoveCnt = (BYTE)sManager->db.GetAsInteger("MoveCnt");
	// ----
	sManager->db.GetAsString("GameID1", CharName[0]);
	sManager->db.GetAsString("GameID2", CharName[1]);
	sManager->db.GetAsString("GameID3", CharName[2]);
	sManager->db.GetAsString("GameID4", CharName[3]);
	sManager->db.GetAsString("GameID5", CharName[4]);
	// ----
	sManager->db.Close();
	// ----
	for(int i = 0 ; i < 5 ; i++)
	{
		if(CharName[i][0] != 0)
		{
			if(strlen(CharName[i]) >= 4)
			{
				if(sManager->db.ExecQuery("SELECT cLevel, Class, CtlCode, DbVersion FROM vCharacterPreview WHERE Name='%s'", CharName[i]) == TRUE)
				{
					char szTemp[512];
					pCL = (SDHP_CHARLIST *)(cBUFFER + lOfs);
					// ----
					sManager->db.Fetch();
					pCL->Level = (WORD)sManager->db.GetAsInteger("cLevel");
					pCL->Class = (BYTE)sManager->db.GetAsInteger("Class");
					// ----
					if (pCL->Level >= 220 && pCount->Magumsa == 0)
					{
						pCount->Magumsa = 1;
					}
					// ----
					if (pCL->Level >= 250 && (pCount->Magumsa == 0 || pCount->Magumsa == 1))
					{
						pCount->Magumsa = 2;
					}
					// ----
					pCL->CtlCode = (BYTE)sManager->db.GetAsInteger("CtlCode");
					pCL->DbVersion = (BYTE)sManager->db.GetAsInteger("DbVersion");
					// ----
					sManager->db.Close();
					// ----
					wsprintf(szTemp, "SELECT Inventory FROM Character WHERE Name='%s'", CharName[i]);
					// ----
					if ( sManager->db.GetAsBinary(szTemp, btInvetory) < sizeof(pCL->dbInventory) ) 
					{
						memset(btInvetory, -1, sizeof(btInvetory));
					}
					sManager->db.Close();
					// ----
					if (pCL->DbVersion >= 3)
					{
						memset(pCL->dbInventory, -1, sizeof(pCL->dbInventory));
						// ----
						for(int i=0;i<12;i++)
						{
							if ( btInvetory[0+i*16] == 0xFF && (btInvetory[7+i*16] & 0x80 ) == 0x80 && (btInvetory[10+i*16] & 0xF0) == 0xF0 )
							{
								pCL->dbInventory[i*4]	= -1;
								pCL->dbInventory[i*4+1] = -1;
								pCL->dbInventory[i*4+2] = -1;
								pCL->dbInventory[i*4+3] = -1;
							}
							else
							{
								pCL->dbInventory[i*4]	 = (btInvetory[0+i*16]);	// 0..7 bits of Item
								pCL->dbInventory[i*4+1]  = (btInvetory[1+i*16]);	// Get Level of Item
								pCL->dbInventory[i*4+2]  = (btInvetory[7+i*16]);	// 8 bit     of Item
								pCL->dbInventory[i*4+3]  = (btInvetory[9+i*16]);	// 9..12 bit of Item
							}
						}
					}
					// ----
					sManager->db.ExecQuery("SELECT G_Status FROM GuildMember WHERE Name='%s'", CharName[i]);
					// ----
					if(sManager->db.Fetch() != SQL_NO_DATA)
					{
						pCL->btGuildStatus = (BYTE)sManager->db.GetAsInteger("G_Status");
					}
					else
					{
						pCL->btGuildStatus = 0xFF;
					}
					// ----
					pCL->Index = i;
					lstrcpy(pCL->Name, CharName[i]);
					// ----
					iCharCount++;
					lOfs+=sizeof(SDHP_CHARLIST);
				}
				// ----
				sManager->db.Close();
			}
		}
	}
	// ----
	PHeadSetW((LPBYTE)pCount, 0x91, sizeof(SDHP_CHARLISTCOUNT) + sizeof(SDHP_CHARLIST) * iCharCount);
	pCount->Count = iCharCount;
	// ----
	sManager->DataSend(aIndex, (LPBYTE)cBUFFER, sizeof(SDHP_CHARLISTCOUNT) + sizeof(SDHP_CHARLIST) * iCharCount);
	
	//SCFExDB Adds
	GDGetAccountExtraInfo(sManager, aIndex,aRecv->Number,szAccountID);
}
//---------------------------------------------------------------------------

void JGCharacterCreateRequest(cDataServer * sManager, int aIndex, SDHP_CREATECHAR * aRecv)
{
	//OutputDebugString("JGCharacterCreateRequest\n");

	SDHP_CREATECHARRESULT pResult	= {0};
	char szAccountID[11]			= {0};
	char szName[11]					= {0};
	TCHAR CharName[5][11]			= {0};
	int iIndex						= 0;
	// ----
	pResult.ClassSkin				= aRecv->ClassSkin;
	pResult.Number					= aRecv->Number;
	pResult.Level					= 1;
	// ----
	//OutputDebugString("memcpy(pResult.AccountId, aRecv->AccountId, 10)\n");
	memcpy(pResult.AccountId, aRecv->AccountId, 10);
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountId, 10)\n");
	memcpy(szAccountID, aRecv->AccountId, 10);
	//OutputDebugString("memcpy(szName, aRecv->Name, 10)\n");
	memcpy(szName, aRecv->Name, 10);

		
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{	//Open Call 1
		if(sManager->db.ExecQuery("SELECT * FROM AccountCharacter WHERE Id='%s'", szAccountID) == TRUE)
		{
			sManager->db.Fetch();
			sManager->db.GetAsString("GameID1", CharName[0]);
			sManager->db.GetAsString("GameID2", CharName[1]);
			sManager->db.GetAsString("GameID3", CharName[2]);
			sManager->db.GetAsString("GameID4", CharName[3]);
			sManager->db.GetAsString("GameID5", CharName[4]);

			for(iIndex = 0 ; iIndex < 5 ; iIndex++)
			{
				if(CharName[iIndex][0] == 0)
				{
					break;
				}
			}

			if ( iIndex < 5 )
			{
				//OutputDebugString("memcpy(pResult.Name, aRecv->Name, 10)\n");
				memcpy(pResult.Name, aRecv->Name, 10);
				pResult.Pos = iIndex;
				sManager->db.Close();	//Close Call 1

				if ( (pResult.ClassSkin >> 4) < 0 || (pResult.ClassSkin >> 4) >  MAX_TYPE_PLAYER - 1 )
				{
					pResult.Result = 0;
				}

				//Open Call 2
				sManager->db.ExecQuery("WZ_CreateCharacter '%s', '%s', '%d'",szAccountID, szName,
					pResult.ClassSkin);
				sManager->db.Fetch();
				pResult.Result = sManager->db.GetResult(0);
				sManager->db.Close();	//Close Call 2

				if ( pResult.Result == 1 )
				{
					//Open Call 3
					sManager->db.ExecQuery("UPDATE AccountCharacter SET GameID%d='%s' WHERE Id='%s'",
						iIndex+1, szName, szAccountID);
					sManager->db.Fetch();
					sManager->db.Close();	//Close Call 3

					//Open Call 4
					sManager->db.ExecQuery("INSERT INTO SCFS5Quest (AccountID, Name) VALUES ('%s','%s')",szAccountID, szName);
					sManager->db.Fetch();
					sManager->db.Close();	//Close Call 4
				}
			}
			else
			{
				sManager->db.Close();	//Close Call 1
				pResult.Result = 2;
			}
		}
		else
		{
			sManager->db.Close();	//Close Call 1
			pResult.Result = 2;
		}
	}
	else
	{
		pResult.Result = 2;
	}

	pResult.h.set(0x94, sizeof(pResult));

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void JGCharDelRequest(cDataServer * sManager, int aIndex, SDHP_CHARDELETE * aRecv)
{
	//OutputDebugString("JGCharDelRequest\n");

	SDHP_CHARDELETERESULT pResult	= {0};
	char szAccountID[11]			= {0};
	char szName[11]					= {0};
	TCHAR CharName[5][11]			= {0};
	int iIndex						= 0;

	pResult.h.set(0x95, sizeof(pResult));
	pResult.Number = aRecv->Number;
	pResult.Result = 1;	

	//OutputDebugString("memcpy(pResult.AccountID, aRecv->AccountID, 10);\n");
	memcpy(pResult.AccountID, aRecv->AccountID, 10);
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);
	//OutputDebugString("memcpy(szName, aRecv->Name, 10);\n");
	memcpy(szName, aRecv->Name, 10);

	if ( lstrlen(aRecv->AccountID) < 1 )
	{
		pResult.Result = 0;
	}

	if ( lstrlen(aRecv->Name) < 1 )
	{
		pResult.Result = 0;
	}

	if ( pResult.Result == 1 )
	{
		//Open Call 1
		if ( sManager->db.ExecQuery("DELETE FROM Character WHERE AccountID='%s' AND Name='%s'",
				szAccountID, szName) == TRUE )
		{
			sManager->db.Fetch();
			sManager->db.Close();	//Close Call 1

			//Open Call 2
			sManager->db.ExecQuery("SELECT * FROM AccountCharacter WHERE Id='%s'", szAccountID);
			sManager->db.Fetch();
			sManager->db.GetAsString("GameID1", CharName[0]);
			sManager->db.GetAsString("GameID2", CharName[1]);
			sManager->db.GetAsString("GameID3", CharName[2]);
			sManager->db.GetAsString("GameID4", CharName[3]);
			sManager->db.GetAsString("GameID5", CharName[4]);
			for(iIndex = 0 ; iIndex < 5 ; iIndex++)
			{
				if ( lstrcmp(CharName[iIndex], szName) == 0)
				{
					break;
				}
			}
			sManager->db.Close();	//Close Call 2

			//Open Call 3
			sManager->db.ExecQuery("UPDATE AccountCharacter SET GameID%d=NULL WHERE Id='%s'", iIndex+1, szAccountID);
			sManager->db.Fetch();
			sManager->db.Close();	//Close Call 3
		}
		else
		{
			sManager->db.Fetch();
			sManager->db.Close();	//Close Call 1
			pResult.Result = 0;
		}
	}

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void JGGetCharacterInfo(cDataServer * sManager, int aIndex, SDHP_DBCHARINFOREQUEST * aRecv)
{
	//OutputDebugString("JGGetCharacterInfo\n");

	SDHP_DBCHAR_INFORESULT pResult		= {0};
	char szAccountID[11]				= {0};
	char szName[11]						= {0};
	char szTemp[256]					= {0};
	// ----
	BYTE btTemp2[256]					= {0};
	BYTE btTemp[INVENTORY_BINARY_SIZE]	= {0};
	// ----
	PHeadSetW((LPBYTE)&pResult, 0x06, sizeof(pResult));
	// ----
	BuxConvert(aRecv->Name, 10);
	// ----
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);
	//OutputDebugString("memcpy(szName, aRecv->Name, 10);\n");
	memcpy(szName, aRecv->Name, 10);
	//OutputDebugString("memcpy(pResult.AccountID, aRecv->AccountID, 10);\n");
	memcpy(pResult.AccountID, aRecv->AccountID, 10);
	//OutputDebugString("memcpy(pResult.Name, aRecv->Name, 10);\n");
	memcpy(pResult.Name, aRecv->Name, 10);
	// ----
	pResult.Number = aRecv->Number;
	pResult.result = 0;
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{	
		//Open Call 1
		sManager->db.ExecQuery("SELECT cLevel, Class, LevelUpPoint, Experience, Strength, Dexterity, Vitality, Energy, Money, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY, MapDir, PkCount, PkLevel, PkTime, CtlCode, DbVersion, Leadership, ChatLimitTime, FruitPoint FROM Character WHERE Name='%s' AND AccountID='%s'", szName,szAccountID);
		if ( sManager->db.Fetch() != SQL_NO_DATA ) 
		{
			pResult.Level			= (short)sManager->db.GetAsInteger("cLevel"); 
			pResult.Class			= (BYTE)sManager->db.GetAsInteger("Class");
			pResult.LevelUpPoint	= sManager->db.GetAsInteger("LevelUpPoint");
			pResult.Exp				= sManager->db.GetAsInteger("Experience");
			pResult.Str				= (short)sManager->db.GetAsInteger("Strength"); 
			pResult.Dex				= (short)sManager->db.GetAsInteger("Dexterity");
			pResult.Vit				= (short)sManager->db.GetAsInteger("Vitality");
			pResult.Energy			= (short)sManager->db.GetAsInteger("Energy");
			pResult.Money			= sManager->db.GetAsInteger("Money");
			pResult.Life			= (WORD)sManager->db.GetAsFloat("Life");
			pResult.MaxLife			= (WORD)sManager->db.GetAsFloat("MaxLife");
			pResult.Mana			= (WORD)sManager->db.GetAsFloat("Mana");
			pResult.MaxMana			= (WORD)sManager->db.GetAsFloat("MaxMana");
			pResult.MapNumber		= (BYTE)sManager->db.GetAsInteger("MapNumber");
			pResult.MapX			= (BYTE)sManager->db.GetAsInteger("MapPosX");
			pResult.MapY			= (BYTE)sManager->db.GetAsInteger("MapPosY");
			pResult.Dir				= (BYTE)sManager->db.GetAsInteger("MapDir");
			pResult.PkCount			= sManager->db.GetAsInteger("PkCount");
			pResult.PkLevel			= sManager->db.GetAsInteger("PkLevel");
			pResult.PkTime			= sManager->db.GetAsInteger("PkTime");
			pResult.CtlCode			= (BYTE)sManager->db.GetAsInteger("CtlCode");
			pResult.DbVersion		= (BYTE)sManager->db.GetAsInteger("DbVersion");
			pResult.Leadership		= (WORD)sManager->db.GetAsInteger("Leadership");
			pResult.ChatLitmitTime	= (WORD)sManager->db.GetAsInteger("ChatLimitTime");
			pResult.iFruitPoint		= sManager->db.GetAsInteger("FruitPoint");

			sManager->db.Close();	//Close Call 1

			wsprintf(szTemp, "SELECT Inventory from Character where Name='%s'", szName);
			//Open Call 2
			//if (sManager->db.GetAsBinary(szTemp, btTemp) < sizeof(pResult.dbInventory))
			if (sManager->db.GetAsBinary(szTemp, btTemp) <= 0)
			{
				memset(btTemp, -1, sizeof(btTemp));
			}
			//OutputDebugString("memcpy(pResult.dbInventory, btTemp, sizeof(pResult.dbInventory));\n");
			memcpy(pResult.dbInventory, btTemp, sizeof(pResult.dbInventory));
			memset(btTemp, 0, sizeof(btTemp));
			sManager->db.Close();	//Close Call 2

			wsprintf(szTemp, "SELECT MagicList from Character where Name='%s'", szName);
			//Open Call 3
			//if (sManager->db.GetAsBinary(szTemp, btTemp) <= 0) 
			if (sManager->db.GetAsBinary(szTemp, btTemp) < sizeof(pResult.dbMagicList)) 
			{
				memset(btTemp, -1, sizeof(btTemp));
			}
			//OutputDebugString("memcpy(pResult.dbMagicList, btTemp, sizeof(pResult.dbMagicList));\n");
			memcpy(pResult.dbMagicList, btTemp, sizeof(pResult.dbMagicList));
			memset(btTemp, 0, sizeof(btTemp));
			sManager->db.Close();	//Close Call 3

			wsprintf(szTemp, "SELECT Quest from Character where Name='%s'", szName);
			//Open Call 4
			if ( sManager->db.GetAsBinary(szTemp, btTemp) < sizeof(pResult.dbQuest) ) 
			{
				memset(btTemp, -1, sizeof(btTemp));
			}
			//OutputDebugString("memcpy(pResult.dbQuest, btTemp, sizeof(pResult.dbQuest));\n");
			memcpy(pResult.dbQuest, btTemp, sizeof(pResult.dbQuest));
			memset(btTemp, 0, sizeof(btTemp));
			sManager->db.Close();	//Close Call 4

			pResult.result = 1;
			//Open Call 5
			sManager->db.ExecQuery("UPDATE AccountCharacter SET GameIDC='%s' WHERE Id='%s'", szName, szAccountID);
			sManager->db.Fetch();
			sManager->db.Close();	//Close Call 5
		} else {
			sManager->db.Close();	//Close Call 1
		}
	}

	sManager->DataSend(aIndex, (unsigned char*)&pResult, sizeof(pResult));
	// ----
	if ( pResult.result == 1 )
	{
		SDHP_SKILLKEYDATA_SEND pSkillData={0};
		// ----
		pSkillData.h.set(0x60, sizeof(pSkillData));
		pSkillData.aIndex = aRecv->Number;
		
		//Call 1
		sManager->db.ExecQuery("SELECT * FROM OptionData WHERE Name='%s'", szName);
		if (sManager->db.Fetch() != SQL_NO_DATA )
		{
			//OutputDebugString("memcpy(pSkillData.Name, aRecv->Name, 10);\n");

			memcpy(pSkillData.Name, aRecv->Name, 10);
			pSkillData.GameOption = (BYTE)sManager->db.GetAsInteger("GameOption");
			pSkillData.QkeyDefine = (BYTE)sManager->db.GetAsInteger("Qkey");
			pSkillData.WkeyDefine = (BYTE)sManager->db.GetAsInteger("Wkey");
			pSkillData.EkeyDefine = (BYTE)sManager->db.GetAsInteger("Ekey");
			pSkillData.RkeyDefine = (BYTE)sManager->db.GetAsInteger("RKey");
			pSkillData.ChatWindow = (BYTE)sManager->db.GetAsInteger("ChatWindow");

			sManager->db.Close();	//Call 1

			wsprintf(szTemp, "SELECT SkillKey from OptionData where Name='%s'", szName);
			//Call 2
			sManager->db.GetAsBinary(szTemp, btTemp2);
			//OutputDebugString("memcpy(pSkillData.SkillKeyBuffer, btTemp2, sizeof(pSkillData.SkillKeyBuffer));\n");
			memcpy(pSkillData.SkillKeyBuffer, btTemp2, sizeof(pSkillData.SkillKeyBuffer));
		}

		sManager->db.Close();	//Call 1 or 2

		sManager->DataSend(aIndex, (unsigned char*)&pSkillData, sizeof(pSkillData));
		
		//SCFExDB AddOn
		GDGetPlayerExtraInfo(sManager, aIndex,aRecv->Number,szName,szAccountID);
	}

}

void JGGetCharacterInfoS6E2(cDataServer * sManager, int aIndex, SDHP_DBCHARINFOREQUEST * aRecv)
{
	//OutputDebugString("JGGetCharacterInfo\n");

	SDHP_DBCHAR_INFORESULTS6E2 pResult	= {0};
	char szAccountID[11]				= {0};
	char szName[11]						= {0};
	char szTemp[256]					= {0};
	// ----
	BYTE btTemp2[256]					= {0};
	BYTE btTemp[INVENTORY_BINARY_SIZE]	= {0};
	// ----
	PHeadSetW((LPBYTE)&pResult, 0x06, sizeof(pResult));
	// ----
	BuxConvert(aRecv->Name, 10);
	// ----
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);
	//OutputDebugString("memcpy(szName, aRecv->Name, 10);\n");
	memcpy(szName, aRecv->Name, 10);
	//OutputDebugString("memcpy(pResult.AccountID, aRecv->AccountID, 10);\n");
	memcpy(pResult.AccountID, aRecv->AccountID, 10);
	//OutputDebugString("memcpy(pResult.Name, aRecv->Name, 10);\n");
	memcpy(pResult.Name, aRecv->Name, 10);
	// ----
	pResult.Number = aRecv->Number;
	pResult.result = 0;
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{	
		//Open Call 1
		sManager->db.ExecQuery("SELECT cLevel, Class, LevelUpPoint, Experience, Strength, Dexterity, Vitality, Energy, Money, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY, MapDir, PkCount, PkLevel, PkTime, CtlCode, DbVersion, Leadership, ChatLimitTime, FruitPoint, SCFExtInventory, SCFUpdatedPShop FROM Character WHERE Name='%s' AND AccountID='%s'", szName,szAccountID);
		if ( sManager->db.Fetch() != SQL_NO_DATA ) 
		{
			pResult.Level			= (short)sManager->db.GetAsInteger("cLevel"); 
			pResult.Class			= (BYTE)sManager->db.GetAsInteger("Class");
			pResult.LevelUpPoint	= sManager->db.GetAsInteger("LevelUpPoint");
			pResult.Exp				= sManager->db.GetAsInteger("Experience");
			pResult.Str				= (short)sManager->db.GetAsInteger("Strength"); 
			pResult.Dex				= (short)sManager->db.GetAsInteger("Dexterity");
			pResult.Vit				= (short)sManager->db.GetAsInteger("Vitality");
			pResult.Energy			= (short)sManager->db.GetAsInteger("Energy");
			pResult.Money			= sManager->db.GetAsInteger("Money");
			pResult.Life			= (WORD)sManager->db.GetAsFloat("Life");
			pResult.MaxLife			= (WORD)sManager->db.GetAsFloat("MaxLife");
			pResult.Mana			= (WORD)sManager->db.GetAsFloat("Mana");
			pResult.MaxMana			= (WORD)sManager->db.GetAsFloat("MaxMana");
			pResult.MapNumber		= (BYTE)sManager->db.GetAsInteger("MapNumber");
			pResult.MapX			= (BYTE)sManager->db.GetAsInteger("MapPosX");
			pResult.MapY			= (BYTE)sManager->db.GetAsInteger("MapPosY");
			pResult.Dir				= (BYTE)sManager->db.GetAsInteger("MapDir");
			pResult.PkCount			= sManager->db.GetAsInteger("PkCount");
			pResult.PkLevel			= sManager->db.GetAsInteger("PkLevel");
			pResult.PkTime			= sManager->db.GetAsInteger("PkTime");
			pResult.CtlCode			= (BYTE)sManager->db.GetAsInteger("CtlCode");
			pResult.DbVersion		= (BYTE)sManager->db.GetAsInteger("DbVersion");
			pResult.Leadership		= (WORD)sManager->db.GetAsInteger("Leadership");
			pResult.ChatLitmitTime	= (WORD)sManager->db.GetAsInteger("ChatLimitTime");
			pResult.iFruitPoint		= sManager->db.GetAsInteger("FruitPoint");
			pResult.ExInventory		= sManager->db.GetAsInteger("SCFExtInventory");
			pResult.UpdatedPShop	= sManager->db.GetAsInteger("SCFUpdatedPShop");
			

			sManager->db.Close();	//Close Call 1

			wsprintf(szTemp, "SELECT Inventory from Character where Name='%s'", szName);
			//Open Call 2
			//if (sManager->db.GetAsBinary(szTemp, btTemp) < sizeof(pResult.dbInventory))
			if (sManager->db.GetAsBinary(szTemp, btTemp) <= 0)
			{
				memset(btTemp, -1, sizeof(btTemp));
			}
			//OutputDebugString("memcpy(pResult.dbInventory, btTemp, sizeof(pResult.dbInventory));\n");
			memcpy(pResult.dbInventory, btTemp, sizeof(pResult.dbInventory));
			memset(btTemp, 0, sizeof(btTemp));
			sManager->db.Close();	//Close Call 2

			wsprintf(szTemp, "SELECT MagicList from Character where Name='%s'", szName);
			//Open Call 3
			//if (sManager->db.GetAsBinary(szTemp, btTemp) <= 0) 
			if (sManager->db.GetAsBinary(szTemp, btTemp) < sizeof(pResult.dbMagicList)) 
			{
				memset(btTemp, -1, sizeof(btTemp));
			}
			//OutputDebugString("memcpy(pResult.dbMagicList, btTemp, sizeof(pResult.dbMagicList));\n");
			memcpy(pResult.dbMagicList, btTemp, sizeof(pResult.dbMagicList));
			memset(btTemp, 0, sizeof(btTemp));
			sManager->db.Close();	//Close Call 3

			wsprintf(szTemp, "SELECT Quest from Character where Name='%s'", szName);
			//Open Call 4
			if ( sManager->db.GetAsBinary(szTemp, btTemp) < sizeof(pResult.dbQuest) ) 
			{
				memset(btTemp, -1, sizeof(btTemp));
			}
			//OutputDebugString("memcpy(pResult.dbQuest, btTemp, sizeof(pResult.dbQuest));\n");
			memcpy(pResult.dbQuest, btTemp, sizeof(pResult.dbQuest));
			memset(btTemp, 0, sizeof(btTemp));
			sManager->db.Close();	//Close Call 4

			pResult.result = 1;
			//Open Call 5
			sManager->db.ExecQuery("UPDATE AccountCharacter SET GameIDC='%s' WHERE Id='%s'", szName, szAccountID);
			sManager->db.Fetch();
			sManager->db.Close();	//Close Call 5
		} else {
			sManager->db.Close();	//Close Call 1
		}
	}

	sManager->DataSend(aIndex, (unsigned char*)&pResult, sizeof(pResult));
	// ----
	if ( pResult.result == 1 )
	{
		SDHP_SKILLKEYDATA_SEND pSkillData={0};
		// ----
		pSkillData.h.set(0x60, sizeof(pSkillData));
		pSkillData.aIndex = aRecv->Number;
		
		//Call 1
		sManager->db.ExecQuery("SELECT * FROM OptionData WHERE Name='%s'", szName);
		if (sManager->db.Fetch() != SQL_NO_DATA )
		{
			//OutputDebugString("memcpy(pSkillData.Name, aRecv->Name, 10);\n");

			memcpy(pSkillData.Name, aRecv->Name, 10);
			pSkillData.GameOption = (BYTE)sManager->db.GetAsInteger("GameOption");
			pSkillData.QkeyDefine = (BYTE)sManager->db.GetAsInteger("QKey");
			pSkillData.WkeyDefine = (BYTE)sManager->db.GetAsInteger("WKey");
			pSkillData.EkeyDefine = (BYTE)sManager->db.GetAsInteger("EKey");
			pSkillData.RkeyDefine = (BYTE)sManager->db.GetAsInteger("RKey");
			pSkillData.ChatWindow = (BYTE)sManager->db.GetAsInteger("ChatWindow");

			sManager->db.Close();	//Call 1

			wsprintf(szTemp, "SELECT SkillKey from OptionData where Name='%s'", szName);
			//Call 2
			sManager->db.GetAsBinary(szTemp, btTemp2);
			//OutputDebugString("memcpy(pSkillData.SkillKeyBuffer, btTemp2, sizeof(pSkillData.SkillKeyBuffer));\n");
			memcpy(pSkillData.SkillKeyBuffer, btTemp2, sizeof(pSkillData.SkillKeyBuffer));
		}

		sManager->db.Close();	//Call 1 or 2

		sManager->DataSend(aIndex, (unsigned char*)&pSkillData, sizeof(pSkillData));
	
		//SCFExDB AddOn
		GDGetPlayerExtraInfo(sManager, aIndex,aRecv->Number,szName,szAccountID);
		//GDGetPlayerMasterSkillInfo(sManager, aIndex,aRecv->Number,szName,szAccountID);
	}

}
//---------------------------------------------------------------------------

void GJSetCharacterInfo(cDataServer * sManager, int aIndex, SDHP_DBCHAR_INFOSAVE * aRecv)
{
	//OutputDebugString("GJSetCharacterInfo\n");

	char szName[11]			= {0};
	char szTemp[128]		= {0};
	BYTE dbInventory[3776]	= {0};
	BYTE dbMagicList[180]	= {0};
	BYTE dbQuest[50]		= {0};

	// ----
	//OutputDebugString("memcpy(szName, aRecv->Name, 10);\n");
	memcpy(szName, aRecv->Name, 10);
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{	
		memcpy(dbInventory, aRecv->dbInventory, sizeof(aRecv->dbInventory));
		memcpy(dbMagicList, aRecv->dbMagicList, sizeof(aRecv->dbMagicList));
		memcpy(dbQuest, aRecv->dbQuest, sizeof(aRecv->dbQuest));
		// ----
		sManager->db.ExecQuery("UPDATE Character SET cLevel=%d,Class=%d,LevelUpPoint=%d, Experience=%d, Strength=%d, Dexterity=%d, Vitality=%d, Energy=%d, Money=%d, Life=%d, MaxLife=%d, Mana=%d, MaxMana=%d, MapNumber=%d, MapPosX=%d, MapPosY=%d, MapDir=%d, PkCount=%d, PkLevel=%d, PkTime=%d, DbVersion=%d, Leadership=%d, ChatLimitTime=%d, FruitPoint=%d WHERE Name = '%s'",
			aRecv->Level, aRecv->Class, aRecv->LevelUpPoint, aRecv->Exp, aRecv->Str, aRecv->Dex, aRecv->Vit, aRecv->Energy, aRecv->Money, aRecv->Life, aRecv->MaxLife, aRecv->Mana, aRecv->MaxMana, aRecv->MapNumber, aRecv->MapX, aRecv->MapY, aRecv->Dir, aRecv->PkCount, aRecv->PkLevel, aRecv->PkTime, aRecv->dbVersion, aRecv->Leadership, aRecv->ChatLitmitTime, aRecv->iFruitPoint, szName);
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		wsprintf(szTemp, "UPDATE Character SET Inventory=? WHERE Name='%s'", szName);
		sManager->db.SetAsBinary(szTemp, dbInventory, sizeof(dbInventory));
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		wsprintf(szTemp, "UPDATE Character SET MagicList=? WHERE Name='%s'", szName);
		sManager->db.SetAsBinary(szTemp, dbMagicList, sizeof(dbMagicList));
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		wsprintf(szTemp, "UPDATE Character SET Quest=? WHERE Name='%s'", szName);
		sManager->db.SetAsBinary(szTemp, dbQuest, sizeof(dbQuest));
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		if (aRecv->CharInfoSave == 1)
		{
			sManager->db.ExecQuery("INSERT INTO T_CurCharName (Name) VALUES('%s')", szName);
			sManager->db.Fetch();
			sManager->db.Close();
		}
	}
}

void GJSetCharacterInfoS6E2(cDataServer * sManager, int aIndex, SDHP_DBCHAR_INFOSAVES6E2 * aRecv)
{
	//OutputDebugString("GJSetCharacterInfo\n");

	char szName[11]			= {0};
	char szTemp[128]		= {0};
	BYTE dbInventory[3776]	= {0};
	BYTE dbMagicList[180]	= {0};
	BYTE dbQuest[50]		= {0};

	// ----
	//OutputDebugString("memcpy(szName, aRecv->Name, 10);\n");
	memcpy(szName, aRecv->Name, 10);
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{	
		memcpy(dbInventory, aRecv->dbInventory, sizeof(aRecv->dbInventory));
		memcpy(dbMagicList, aRecv->dbMagicList, sizeof(aRecv->dbMagicList));
		memcpy(dbQuest, aRecv->dbQuest, sizeof(aRecv->dbQuest));
		// ----
		sManager->db.ExecQuery("UPDATE Character SET cLevel=%d,Class=%d,LevelUpPoint=%d, Experience=%d, Strength=%d, Dexterity=%d, Vitality=%d, Energy=%d, Money=%d, Life=%d, MaxLife=%d, Mana=%d, MaxMana=%d, MapNumber=%d, MapPosX=%d, MapPosY=%d, MapDir=%d, PkCount=%d, PkLevel=%d, PkTime=%d, DbVersion=%d, Leadership=%d, ChatLimitTime=%d, FruitPoint=%d, SCFExtInventory=%d, SCFUpdatedPShop=1 WHERE Name = '%s'",
			aRecv->Level, aRecv->Class, aRecv->LevelUpPoint, aRecv->Exp, aRecv->Str, aRecv->Dex, aRecv->Vit, aRecv->Energy, aRecv->Money, aRecv->Life, aRecv->MaxLife, aRecv->Mana, aRecv->MaxMana, aRecv->MapNumber, aRecv->MapX, aRecv->MapY, aRecv->Dir, aRecv->PkCount, aRecv->PkLevel, aRecv->PkTime, aRecv->dbVersion, aRecv->Leadership, aRecv->ChatLitmitTime, aRecv->iFruitPoint, aRecv->ExInventory, szName);
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		wsprintf(szTemp, "UPDATE Character SET Inventory=? WHERE Name='%s'", szName);
		sManager->db.SetAsBinary(szTemp, dbInventory, sizeof(dbInventory));
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		wsprintf(szTemp, "UPDATE Character SET MagicList=? WHERE Name='%s'", szName);
		sManager->db.SetAsBinary(szTemp, dbMagicList, sizeof(dbMagicList));
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		wsprintf(szTemp, "UPDATE Character SET Quest=? WHERE Name='%s'", szName);
		sManager->db.SetAsBinary(szTemp, dbQuest, sizeof(dbQuest));
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		if (aRecv->CharInfoSave == 1)
		{
			sManager->db.ExecQuery("INSERT INTO T_CurCharName (Name) VALUES('%s')", szName);
			sManager->db.Fetch();
			sManager->db.Close();
		}
	}
}
//---------------------------------------------------------------------------

void DGGetWarehouseList(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB * aRecv)
{
	//OutputDebugString("DGGetWarehouseList\n");

	SDHP_GETWAREHOUSEDB_SAVE pResult	= {0};
	SDHP_GETWAREHOUSEDB_RESULT pMsg		= {0};
	char szAccountID[11]				= {0};
	char szTemp[128]					= {0};
	// ----
	PHeadSetW((LPBYTE)&pResult, 0x08, sizeof(pResult));
	pMsg.h.set(0x10, sizeof(pMsg));
	// ----
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);
	//OutputDebugString("memcpy(pResult.AccountID, szAccountID, 10);\n");
	memcpy(pResult.AccountID, szAccountID, 10);
	//OutputDebugString("memcpy(pMsg.AccountID, szAccountID, 10);\n");
	memcpy(pMsg.AccountID, szAccountID, 10);
	// ----
	pMsg.aIndex	   = aRecv->aIndex;
	pResult.aIndex = aRecv->aIndex;
	// ----
	BOOL bRet = sManager->db.ExecQuery("SELECT AccountID FROM warehouse WHERE AccountID='%s'", szAccountID);
	// ----
	if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
	{
		sManager->db.Close();
		sManager->db.ExecQuery("INSERT INTO warehouse (AccountID, Money, EndUseDate, DbVersion) VALUES ('%s',0, getdate(), 3)", szAccountID);
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		sManager->DataSend(aIndex, (unsigned char*)&pMsg, pMsg.h.size);
		// ----
		return;
	}
	// ----
	sManager->db.Close();
	// ----
	sManager->db.ExecQuery("SELECT Money, DbVersion, pw FROM warehouse WHERE AccountID='%s'", szAccountID);
	sManager->db.Fetch();
	// ----
	pResult.Money		= sManager->db.GetAsInteger("Money");
	pResult.DbVersion	= sManager->db.GetAsInteger("DbVersion");
	pResult.pw			= sManager->db.GetAsInteger("pw");
	// ----
	sManager->db.Close();
	// ----
	wsprintf(szTemp, "SELECT Items FROM warehouse WHERE AccountID='%s'", szAccountID);
	// ----
	if ( sManager->db.GetAsBinary(szTemp, pResult.dbItems) <= 0 )
	{
		memset(pResult.dbItems, -1, sizeof(pResult.dbItems));
	}
	// ----
	sManager->db.Close();
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, sizeof(pResult));
}

void DGGetWarehouseListS6E2(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB * aRecv)
{
	//OutputDebugString("DGGetWarehouseList\n");

	SDHP_GETWAREHOUSEDB_S6E2_SAVE pResult	= {0};
	SDHP_GETWAREHOUSEDB_RESULT pMsg		= {0};
	char szAccountID[11]				= {0};
	char szTemp[128]					= {0};
	// ----
	PHeadSetW((LPBYTE)&pResult, 0x08, sizeof(pResult));
	pMsg.h.set(0x10, sizeof(pMsg));
	// ----
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);
	//OutputDebugString("memcpy(pResult.AccountID, szAccountID, 10);\n");
	memcpy(pResult.AccountID, szAccountID, 10);
	//OutputDebugString("memcpy(pMsg.AccountID, szAccountID, 10);\n");
	memcpy(pMsg.AccountID, szAccountID, 10);
	// ----
	pMsg.aIndex	   = aRecv->aIndex;
	pResult.aIndex = aRecv->aIndex;

	memset(pResult.dbItems, -1, sizeof(pResult.dbItems));
	// ----
	BOOL bRet = sManager->db.ExecQuery("SELECT AccountID FROM warehouse WHERE AccountID='%s'", szAccountID);
	// ----
	if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
	{
		sManager->db.Close();
		sManager->db.ExecQuery("INSERT INTO warehouse (AccountID, Money, EndUseDate, DbVersion) VALUES ('%s',0, getdate(), 3)", szAccountID);
		sManager->db.Fetch();
		sManager->db.Close();
		// ----
		sManager->DataSend(aIndex, (unsigned char*)&pMsg, pMsg.h.size);
		// ----
		return;
	}
	// ----
	sManager->db.Close();
	// ----
	sManager->db.ExecQuery("SELECT Money, DbVersion, pw FROM warehouse WHERE AccountID='%s'", szAccountID);
	sManager->db.Fetch();
	// ----
	pResult.Money		= sManager->db.GetAsInteger("Money");
	pResult.DbVersion	= sManager->db.GetAsInteger("DbVersion");
	pResult.pw			= sManager->db.GetAsInteger("pw");
	// ----
	sManager->db.Close();
	// ----
	wsprintf(szTemp, "SELECT Items FROM warehouse WHERE AccountID='%s'", szAccountID);
	// ----
	if ( sManager->db.GetAsBinary(szTemp, pResult.dbItems) <= 0 )
	{
		memset(pResult.dbItems, -1, sizeof(pResult.dbItems));
	}
	// ----
	sManager->db.Close();
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, sizeof(pResult));
}

//---------------------------------------------------------------------------

void GDSetWarehouseList(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB_SAVE * aRecv)
{
	//OutputDebugString("GDSetWarehouseList\n");

	char szAccountID[11]	= {0};
	char szTemp[128]		= {0};
	// ----
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);
	// ----
	sManager->db.ExecQuery("UPDATE warehouse SET Money=%d, pw=%d, DbVersion=%d WHERE AccountID='%s'",
		aRecv->Money, aRecv->pw, aRecv->DbVersion, szAccountID);
	sManager->db.Fetch();
	sManager->db.Close();
	// ----
	wsprintf(szTemp, "UPDATE warehouse SET Items=? WHERE AccountID='%s'", szAccountID);
	// ----
	sManager->db.SetAsBinary(szTemp, aRecv->dbItems, sizeof(aRecv->dbItems));
	sManager->db.Fetch();
	sManager->db.Close();
}

void GDSetWarehouseListS6E2(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB_S6E2_SAVE * aRecv)
{
	//OutputDebugString("GDSetWarehouseList\n");

	char szAccountID[11]	= {0};
	char szTemp[128]		= {0};
	// ----
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);
	// ----
	sManager->db.ExecQuery("UPDATE warehouse SET Money=%d, pw=%d, DbVersion=%d WHERE AccountID='%s'",
		aRecv->Money, aRecv->pw, aRecv->DbVersion, szAccountID);
	sManager->db.Fetch();
	sManager->db.Close();
	// ----
	wsprintf(szTemp, "UPDATE warehouse SET Items=? WHERE AccountID='%s'", szAccountID);
	// ----
	sManager->db.SetAsBinary(szTemp, aRecv->dbItems, sizeof(aRecv->dbItems));
	sManager->db.Fetch();
	sManager->db.Close();
}

//---------------------------------------------------------------------------

void GDUserItemSave(cDataServer * sManager, int aIndex, SDHP_DBCHAR_ITEMSAVE * aRecv)
{
	//OutputDebugString("GDUserItemSave\n");

	char szName[11]		= {0};
	char szTemp[128]	= {0};
	//OutputDebugString("memcpy(szName, aRecv->Name, 10);\n");
	memcpy(szName, aRecv->Name, 10);
	// ----
	//sManager->db.ExecQuery("UPDATE Character SET DbVersion=%d WHERE Name = '%s'", szName);
	//sManager->db.Fetch();
	//sManager->db.Close();
	// ----
	wsprintf(szTemp, "UPDATE Character SET Inventory=? WHERE Name='%s'", szName);
	// ----
	sManager->db.SetAsBinary(szTemp, aRecv->dbInventory, sizeof(aRecv->dbInventory));
	sManager->db.Fetch();
	sManager->db.Close();
}


//---------------------------------------------------------------------------

void GDSetWarehouseMoney(cDataServer * sManager, int aIndex, SDHP_WAREHOUSEMONEY_SAVE * aRecv)
{
	//OutputDebugString("GDSetWarehouseMoney\n");

	char szAccountID[11]	= {0};
	// ----
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);
	// ----
	sManager->db.ExecQuery("UPDATE warehouse SET Money=%d WHERE AccountID='%s'", aRecv->Money, szAccountID);
	sManager->db.Fetch();
	sManager->db.Close();
}
//---------------------------------------------------------------------------

void ItemSerialCreateRecv(cDataServer * sManager, int aIndex, SDHP_ITEMCREATE * aRecv)
{
	//OutputDebugString("ItemSerialCreateRecv\n");

	SDHP_ITEMCREATERECV pResult = {0};
	int					Number	= 0;
	// ----
	pResult.h.set(0x52, sizeof(pResult));
	// ----
	sManager->db.ExecQuery("EXEC WZ_GetItemSerial");
	sManager->db.Fetch();
	// ----
	Number				= sManager->db.GetResult(0);
	// ----
	sManager->db.Close();
	// ----
	pResult.m_Number	= Number;
	pResult.aIndex		= aRecv->aIndex;
	pResult.Dur			= aRecv->Dur;
	pResult.Level		= aRecv->Level;
	pResult.lootindex	= aRecv->lootindex;
	pResult.MapNumber	= aRecv->MapNumber;
	pResult.NewOption	= aRecv->NewOption;
	pResult.Op1			= aRecv->Op1;
	pResult.Op2			= aRecv->Op2;
	pResult.Op3			= aRecv->Op3;
	pResult.SetOption	= aRecv->SetOption;
	pResult.Type		= aRecv->Type;
	pResult.x			= aRecv->x;
	pResult.y			= aRecv->y;
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void PetItemSerialCreateRecv(cDataServer * sManager, int aIndex, SDHP_ITEMCREATE * aRecv)
{
	//OutputDebugString("PetItemSerialCreateRecv\n");

	SDHP_ITEMCREATERECV pResult = {0};
	int					Number	= 0;
	// ----
	pResult.h.set(0x52, sizeof(pResult));
	// ----
	sManager->db.ExecQuery("EXEC WZ_GetItemSerial");
	sManager->db.Fetch();
	// ----
	Number = sManager->db.GetResult(0);
	// ----
	sManager->db.Close();
	// ----
	sManager->db.ExecQuery("INSERT INTO T_PetItem_Info (ItemSerial, Pet_Level, Pet_Exp) VALUES (%d, %d, %d)",
		Number, 1, 0);
	sManager->db.Fetch();
	sManager->db.Close();
	// ----
	pResult.m_Number	= Number;
	pResult.aIndex		= aRecv->aIndex;
	pResult.Dur			= aRecv->Dur;
	pResult.Level		= aRecv->Level;
	pResult.lootindex	= aRecv->lootindex;
	pResult.MapNumber	= aRecv->MapNumber;
	pResult.NewOption	= aRecv->NewOption;
	pResult.Op1			= aRecv->Op1;
	pResult.Op2			= aRecv->Op2;
	pResult.Op3			= aRecv->Op3;
	pResult.SetOption	= aRecv->SetOption;
	pResult.Type		= aRecv->Type;
	pResult.x			= aRecv->x;
	pResult.y			= aRecv->y;
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void DGRecvPetItemInfo(cDataServer * sManager, int aIndex, SDHP_REQUEST_PETITEM_INFO * aRecv)
{
	__try
	{
	//OutputDebugString("DGRecvPetItemInfo\n");

	int lOfs1				= sizeof(SDHP_REQUEST_PETITEM_INFO);
	int lOfs2				= sizeof(SDHP_RECV_PETITEM_INFO);

	char szAccountID[11]	= {0};
	char cBUFFER[1000]		= {0};

	Request_PetItem_Info	* pRequestPetInfo	= {0};
	Recv_PetItem_Info		* pRecvPetInfo		= {0};
	SDHP_RECV_PETITEM_INFO	* pRecvPetInfoCount = (SDHP_RECV_PETITEM_INFO *)(cBUFFER);

	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);

	for(int i = 0; i < aRecv->nCount ; i++)
	{
		pRequestPetInfo = (Request_PetItem_Info *)((LPBYTE)aRecv + lOfs1);
		pRecvPetInfo	= (Recv_PetItem_Info *   )((LPBYTE)cBUFFER + lOfs2);

		//Open Call 1
		sManager->db.ExecQuery("SELECT Pet_Level, Pet_Exp FROM T_PetItem_Info WHERE ItemSerial=%d",
			pRequestPetInfo->nSerial);
		if (sManager->db.Fetch() == SQL_NO_DATA)
		{
			sManager->db.Close();	//Close Call 1

			//Open Call 2
			sManager->db.ExecQuery("INSERT INTO T_PetItem_Info (ItemSerial, Pet_Level, Pet_Exp) VALUES (%d, %d, %d)",
				pRequestPetInfo->nSerial, 1, 0);
			sManager->db.Fetch();
			sManager->db.Close();	//Close Call 2

			//Open Call 3
			sManager->db.ExecQuery("SELECT Pet_Level, Pet_Exp FROM T_PetItem_Info WHERE ItemSerial=%d",
				pRequestPetInfo->nSerial);
			sManager->db.Fetch();
		}

		pRecvPetInfo->Level		= sManager->db.GetAsInteger("Pet_Level");
		pRecvPetInfo->Exp		= sManager->db.GetAsInteger("Pet_Exp");
		pRecvPetInfo->nPos		= pRequestPetInfo->nPos;
		pRecvPetInfo->nSerial	= pRequestPetInfo->nSerial;

		sManager->db.Close();	//Close Call 1,3
		pRecvPetInfoCount->nCount++;
		
		lOfs1 += sizeof(Request_PetItem_Info);
		lOfs2 += sizeof(Recv_PetItem_Info);
	}
	// ----
	PHeadSetW((LPBYTE)pRecvPetInfoCount, 0x56, sizeof(SDHP_RECV_PETITEM_INFO) + pRecvPetInfoCount->nCount * sizeof(Recv_PetItem_Info));
	// ----
	pRecvPetInfoCount->InvenType	= aRecv->InvenType;
	pRecvPetInfoCount->Number		= aRecv->Number;
	// ----
	//OutputDebugString("memcpy(pRecvPetInfoCount->AccountID, szAccountID, 10);\n");
	memcpy(pRecvPetInfoCount->AccountID, szAccountID, 10);
	// ----
	sManager->DataSend(aIndex, (unsigned char*)cBUFFER, sizeof(SDHP_RECV_PETITEM_INFO) + pRecvPetInfoCount->nCount * sizeof(Recv_PetItem_Info));
	return;
	}__except( EXCEPTION_ACCESS_VIOLATION == GetExceptionCode() )
	{
		sManager->db.Close();	//Close Call 1
	}
}
//---------------------------------------------------------------------------

void GDSavePetItemInfo(cDataServer * sManager, int aIndex, SDHP_SAVE_PETITEM_INFO * aRecv) 
{
	//OutputDebugString("GDSavePetItemInfo\n");

	int lOfs = sizeof(SDHP_SAVE_PETITEM_INFO);

	for(int i = 0 ; i < aRecv->nCount ; i++)
	{
		Save_PetItem_Info * pSavePetInfo = (Save_PetItem_Info *)((LPBYTE)aRecv + lOfs);
		// ----
		sManager->db.ExecQuery("UPDATE T_PetItem_Info SET Pet_Level=%d, Pet_Exp=%d WHERE ItemSerial=%d",
			pSavePetInfo->Level, pSavePetInfo->Exp, pSavePetInfo->nSerial);
		sManager->db.Fetch();
		sManager->db.Close();

		lOfs += sizeof(Save_PetItem_Info);
	}
}
//---------------------------------------------------------------------------

void DGOptionDataRecv(cDataServer * sManager, int aIndex, SDHP_SKILLKEYDATA * aRecv) 
{
	//OutputDebugString("DGOptionDataRecv\n");

	char szName[11] =	{0};
	char szTemp[100]=	{0};

	//OutputDebugString("memcpy(szName, aRecv->Name, 10);\n");
	memcpy(szName, aRecv->Name, 10);

	//Open Call 1
	sManager->db.ExecQuery("SELECT Name FROM OptionData WHERE Name='%s'", szName);
	if (sManager->db.Fetch() == SQL_NO_DATA)
	{
		sManager->db.Close();	//Close Call 1

		//Open Call 2
		sManager->db.ExecQuery("INSERT INTO OptionData (Name, GameOption, Qkey, Wkey, Ekey, Rkey, ChatWindow) VALUES ('%s', %d, %d, %d, %d, %d, %d)",
			szName, aRecv->GameOption, aRecv->QkeyDefine, aRecv->WkeyDefine, aRecv->EkeyDefine, aRecv->RkeyDefine, aRecv->ChatWindow);
		sManager->db.Fetch();
		sManager->db.Close();	//Close Call 2
	}
	else
	{
		sManager->db.Close();	//Close Call 1

		//Open Call 3
		sManager->db.ExecQuery("UPDATE OptionData SET GameOption=%d, Qkey=%d, Wkey=%d, Ekey=%d, Rkey=%d, ChatWindow=%d WHERE Name='%s'",
			aRecv->GameOption, aRecv->QkeyDefine, aRecv->WkeyDefine, aRecv->EkeyDefine, aRecv->RkeyDefine, aRecv->ChatWindow, szName);
		sManager->db.Fetch();
		sManager->db.Close();	//Close Call 3
	}
	wsprintf(szTemp, "UPDATE OptionData SET SkillKey=? where Name='%s'", szName);

	//Open Call 4
	sManager->db.SetAsBinary(szTemp, aRecv->SkillKeyBuffer, sizeof(aRecv->SkillKeyBuffer));
	sManager->db.Fetch();
	sManager->db.Close();	//Close Call 4
}
//---------------------------------------------------------------------------

void DGMoveOtherServer(cDataServer * sManager, int aIndex, SDHP_CHARACTER_TRANSFER * aRecv) 
{
	LogAddC(2, "[DGMoveOtherServer] :: Error Character Transfer is not longer avaible");
}
//---------------------------------------------------------------------------

void GS_DGAnsOwnerGuildMaster(cDataServer * sManager, int aIndex, CSP_REQ_OWNERGUILDMASTER * aRecv)
{
	//OutputDebugString("GS_DGAnsOwnerGuildMaster\n");

	CSP_ANS_OWNERGUILDMASTER pResult = {0};
	BOOL bRet						 = FALSE;
	// ----
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x01, sizeof(pResult));
	// ----
	pResult.iIndex		= aRecv->iIndex;
	pResult.wMapSvrNum	= aRecv->wMapSvrNum;
	pResult.iResult		= 0;
	// ----
	bRet = sManager->db.ExecQuery("EXEC WZ_CS_GetOwnerGuildMaster %d",aRecv->wMapSvrNum);
	// ----
	if ((bRet == TRUE) && (sManager->db.Fetch() != SQL_NO_DATA))
	{
		pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		// ----
		sManager->db.GetAsString("OwnerGuild", pResult.szCastleOwnGuild);
		sManager->db.GetAsString("OwnerGuildMaster", pResult.szCastleOwnGuildMaster);
	}
	// ----
	sManager->db.Close();
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsCastleNpcBuy(cDataServer * sManager, int aIndex, CSP_REQ_NPCBUY * aRecv)
{
	//OutputDebugString("GS_DGAnsCastleNpcBuy\n");

	CSP_ANS_NPCBUY pResult	= {0};
	BOOL bRet				= FALSE;
	// ----
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x03, sizeof(pResult));
	// ----
	pResult.iBuyCost	= aRecv->iBuyCost;
	pResult.iIndex		= aRecv->iIndex;
	pResult.iNpcIndex	= aRecv->iNpcIndex;
	pResult.iNpcNumber	= aRecv->iNpcNumber;
	pResult.wMapSvrNum	= aRecv->wMapSvrNum;
	pResult.iResult		= 0;
	// ----
	bRet = sManager->db.ExecQuery("EXEC WZ_CS_ReqNpcBuy %d,%d,%d,%d,%d,%d,%d,%d,%d,%d",aRecv->wMapSvrNum,aRecv->iNpcNumber,aRecv->iNpcIndex,aRecv->iNpcDfLevel,aRecv->iNpcRgLevel,aRecv->iNpcMaxHp,aRecv->iNpcHp,aRecv->btNpcX,aRecv->btNpcY,aRecv->btNpcDIR);
	// ----
	if ((bRet == TRUE) && (sManager->db.Fetch() != SQL_NO_DATA))
	{
		pResult.iResult = sManager->db.GetAsInteger("QueryResult");
	}
	// ----
	sManager->db.Close();
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsCastleNpcRepair(cDataServer * sManager, int aIndex, CSP_REQ_NPCREPAIR * aRecv)
{
	//OutputDebugString("GS_DGAnsCastleNpcRepair\n");

	CSP_ANS_NPCREPAIR pResult	= {0};
	BOOL bRet					= FALSE;
	// ----
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x04, sizeof(pResult));
	// ----
	pResult.iIndex		= aRecv->iIndex;
	pResult.iNpcIndex	= aRecv->iNpcIndex;
	pResult.iNpcNumber	= aRecv->iNpcNumber;
	pResult.iRepairCost = aRecv->iRepairCost;
	pResult.wMapSvrNum	= aRecv->wMapSvrNum;
	pResult.iResult		= 0;
	pResult.iNpcMaxHp	= 0;
	pResult.iNpcHpl		= 0;
	// ----
	bRet = sManager->db.ExecQuery("EXEC WZ_CS_ReqNpcRepair %d,%d,%d",aRecv->wMapSvrNum,aRecv->iNpcNumber,aRecv->iNpcIndex);
	// ----
	if ((bRet == TRUE) && (sManager->db.Fetch() != SQL_NO_DATA))
	{
		pResult.iResult		= sManager->db.GetAsInteger("QueryResult");
		pResult.iNpcHpl		= sManager->db.GetAsInteger("NPC_HP");
		pResult.iNpcMaxHp	= sManager->db.GetAsInteger("NPC_MAXHP");
	}
	// ----
	sManager->db.Close();
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsCastleNpcUpgrade(cDataServer * sManager, int aIndex, CSP_REQ_NPCUPGRADE * aRecv)
{
	//OutputDebugString("GS_DGAnsCastleNpcUpgrade\n");

	CSP_ANS_NPCUPGRADE pResult	= {0};
	BOOL bRet					= FALSE;
	// ----
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x05, sizeof(pResult));
	// ----
	pResult.iIndex		= aRecv->iIndex;
	pResult.iNpcIndex	= aRecv->iNpcIndex;
	pResult.iNpcNumber	= aRecv->iNpcNumber;
	pResult.iNpcUpIndex = aRecv->iNpcUpIndex;
	pResult.iNpcUpType	= aRecv->iNpcUpType;
	pResult.iNpcUpValue = aRecv->iNpcUpValue;
	pResult.wMapSvrNum	= aRecv->wMapSvrNum;
	pResult.iResult		= 0;
	// ----
	bRet = sManager->db.ExecQuery("EXEC WZ_CS_ReqNpcUpgrade %d,%d,%d,%d,%d",aRecv->wMapSvrNum,aRecv->iNpcNumber,aRecv->iNpcIndex,aRecv->iNpcUpType,aRecv->iNpcUpValue);
	// ----
	if ((bRet == TRUE) && (sManager->db.Fetch() != SQL_NO_DATA))
	{
		pResult.iResult = sManager->db.GetAsInteger("QueryResult");
	}
	// ----
	sManager->db.Close();
	// ----
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsTaxInfo(cDataServer * sManager, int aIndex, CSP_REQ_TAXINFO * aRecv) 
{
	//OutputDebugString("GS_DGAnsTaxInfo\n");

	CSP_ANS_TAXINFO pResult	= {0};
	BOOL bRet				= FALSE;
	// ----
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x06, sizeof(pResult));
	// ----
	pResult.iIndex			= aRecv->iIndex;
	pResult.wMapSvrNum		= aRecv->wMapSvrNum;
	pResult.i64CastleMoney	= 0;
	pResult.iResult			= 0;
	pResult.iTaxHuntZone	= 0;
	pResult.iTaxRateChaos	= 0;
	pResult.iTaxRateStore	= 0;

	bRet = sManager->db.ExecQuery("EXEC WZ_CS_GetCastleTaxInfo %d",aRecv->wMapSvrNum);
	if ((bRet == TRUE) && (sManager->db.Fetch() != SQL_NO_DATA))
	{
		pResult.iResult			= 1;
		pResult.i64CastleMoney	= sManager->db.GetAsInteger64("MONEY");
		pResult.iTaxRateChaos	= sManager->db.GetAsInteger("TAX_RATE_CHAOS");
		pResult.iTaxRateStore	= sManager->db.GetAsInteger("TAX_RATE_STORE");
		pResult.iTaxHuntZone	= sManager->db.GetAsInteger("TAX_HUNT_ZONE");
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsTaxRateChange(cDataServer * sManager, int aIndex, CSP_REQ_TAXRATECHANGE * aRecv) 
{
	//OutputDebugString("GS_DGAnsTaxRateChange\n");

	CSP_ANS_TAXRATECHANGE pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x07, sizeof(pResult));
	
	pResult.iIndex = aRecv->iIndex;
	pResult.iResult = 0;
	pResult.iTaxKind = aRecv->iTaxKind;
	pResult.iTaxRate = aRecv->iTaxRate;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ModifyTaxRate %d,%d,%d",aRecv->wMapSvrNum,aRecv->iTaxKind,aRecv->iTaxRate);
	if (bRet == TRUE )
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{			
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
			pResult.iTaxKind = sManager->db.GetAsInteger("TaxKind");
			pResult.iTaxRate = sManager->db.GetAsInteger("TaxRate");
		}
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsCastleMoneyChange(cDataServer * sManager, int aIndex, CSP_REQ_MONEYCHANGE * aRecv) 
{
	//OutputDebugString("GS_DGAnsCastleMoneyChange\n");

	CSP_ANS_MONEYCHANGE pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x08, sizeof(pResult));
	
	pResult.i64CastleMoney = 0;
	pResult.iIndex = aRecv->iIndex;
	pResult.iMoneyChanged = aRecv->iMoneyChanged;
	pResult.iResult = 0;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ModifyMoney %d,%d",aRecv->wMapSvrNum,aRecv->iMoneyChanged);
	if (bRet == TRUE )
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{			
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
			pResult.i64CastleMoney = sManager->db.GetAsInteger64("MONEY");
		}
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsSiegeDateChange(cDataServer * sManager, int aIndex, CSP_REQ_SDEDCHANGE * aRecv) 
{
	//OutputDebugString("GS_DGAnsSiegeDateChange\n");

	CSP_ANS_SDEDCHANGE pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x09, sizeof(pResult));
	
	pResult.btEndDay = aRecv->btEndDay;
	pResult.btEndMonth = aRecv->btEndMonth;
	pResult.btStartDay = aRecv->btStartDay;
	pResult.btStartMonth = aRecv->btStartMonth;
	pResult.iIndex = aRecv->iIndex;
	pResult.iResult = 0;
	pResult.wEndYear = aRecv->wEndYear;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	pResult.wStartYear = aRecv->wStartYear;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ModifyCastleSchedule %d, '%d-%d-%d 00:00:00', '%d-%d-%d 00:00:00'",aRecv->wMapSvrNum,aRecv->wStartYear,aRecv->btStartMonth,aRecv->btStartDay,aRecv->wEndYear,aRecv->btEndMonth,aRecv->btEndDay);
	if (bRet == TRUE )
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{			
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsGuildMarkRegInfo(cDataServer * sManager, int aIndex, CSP_REQ_GUILDREGINFO * aRecv) 
{
	//OutputDebugString("GS_DGAnsGuildMarkRegInfo\n");

	CSP_ANS_GUILDREGINFO pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x0A, sizeof(pResult));
	
	pResult.bIsGiveUp = 0;
	pResult.btRegRank = 0;
	pResult.iIndex = aRecv->iIndex;
	pResult.iRegMarkCount = 0;
	pResult.iResult = 0;
	//OutputDebugString("memcpy(pResult.szGuildName,aRecv->szGuildName,8);\n");
	
	char szGuild[9]		= {0};
	if(strlen(aRecv->szGuildName) >= 8)
		strncpy(szGuild,aRecv->szGuildName,8);
	else
		strcpy(szGuild, aRecv->szGuildName);

	strcpy(pResult.szGuildName,szGuild);
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_GetGuildMarkRegInfo %d, '%s'",aRecv->wMapSvrNum,szGuild);
	if (bRet == TRUE )
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = 1;
			pResult.iRegMarkCount = sManager->db.GetAsInteger("REG_MARKS");
			pResult.btRegRank = (BYTE) sManager->db.GetAsInteger("SEQ_NUM");
			pResult.bIsGiveUp = sManager->db.GetAsInteger("IS_GIVEUP");
		}
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsSiegeEndedChange(cDataServer * sManager, int aIndex, CSP_REQ_SIEGEENDCHANGE * aRecv) 
{
	//OutputDebugString("GS_DGAnsSiegeEndedChange\n");

	CSP_ANS_SIEGEENDCHANGE pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x0B, sizeof(pResult));
	
	pResult.bIsSiegeEnded = aRecv->bIsSiegeEnded;
	pResult.iResult = 0;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ModifySiegeEnd %d, %d",aRecv->wMapSvrNum,aRecv->bIsSiegeEnded);
	if (bRet == TRUE )
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsCastleOwnerChange(cDataServer * sManager, int aIndex, CSP_REQ_CASTLEOWNERCHANGE * aRecv) 
{
	//OutputDebugString("GS_DGAnsCastleOwnerChange\n");

	CSP_ANS_CASTLEOWNERCHANGE pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x0C, sizeof(pResult));
	
	pResult.bIsCastleOccupied = aRecv->bIsCastleOccupied;
	pResult.iResult = 0;
	//OutputDebugString("memcpy(pResult.szOwnerGuildName,aRecv->szOwnerGuildName,8);\n");
	memcpy(pResult.szOwnerGuildName,aRecv->szOwnerGuildName,8);
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ModifyCastleOwnerInfo %d, %d, '%s'",aRecv->wMapSvrNum,aRecv->bIsCastleOccupied,aRecv->szOwnerGuildName);
	if (bRet == TRUE )
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsRegAttackGuild(cDataServer * sManager, int aIndex, CSP_REQ_REGATTACKGUILD * aRecv) 
{
	//OutputDebugString("GS_DGAnsRegAttackGuild\n");

	CSP_ANS_REGATTACKGUILD pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x0D, sizeof(pResult));
	
	pResult.iIndex = aRecv->iIndex;
	pResult.iResult = 0;
	//OutputDebugString("memcpy(pResult.szEnemyGuildName,aRecv->szEnemyGuildName,8);\n");
	memcpy(pResult.szEnemyGuildName,aRecv->szEnemyGuildName,8);
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ReqRegAttackGuild %d, '%s'",aRecv->wMapSvrNum,aRecv->szEnemyGuildName);
	if (bRet == TRUE )
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsRestartCastleState(cDataServer * sManager, int aIndex, CSP_REQ_CASTLESIEGEEND * aRecv) 
{
	//OutputDebugString("GS_DGAnsRestartCastleState\n");

	CSP_ANS_CASTLESIEGEEND pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x0E, sizeof(pResult));
	
	pResult.iResult = 0;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ResetCastleSiege %d",aRecv->wMapSvrNum);
	if (bRet == TRUE )
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsMapSvrMsgMultiCast(cDataServer * sManager, int aIndex, CSP_REQ_MAPSVRMULTICAST * aRecv) 
{
	//OutputDebugString("GS_DGAnsMapSvrMsgMultiCast\n");

	CSP_ANS_MAPSVRMULTICAST pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x0F, sizeof(pResult));
	
	//OutputDebugString("memcpy(pResult.szMsgText,aRecv->szMsgText,128);\n");
	memcpy(pResult.szMsgText,aRecv->szMsgText,128);
	pResult.wMapSvrNum = aRecv->wMapSvrNum;

	sManager->DataSendAll((unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsRegGuildMark(cDataServer * sManager, int aIndex, CSP_REQ_GUILDREGMARK * aRecv) 
{
	//OutputDebugString("GS_DGAnsRegGuildMark\n");

	CSP_ANS_GUILDREGMARK pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x10, sizeof(pResult));
	
	pResult.iIndex = aRecv->iIndex;
	pResult.iItemPos = aRecv->iItemPos;
	pResult.iRegMarkCount = 0;
	pResult.iResult = 0;
	//OutputDebugString("memcpy(pResult.szGuildName,aRecv->szGuildName,8);\n");
	memcpy(pResult.szGuildName,aRecv->szGuildName,8);
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ReqRegGuildMark %d, '%s'",aRecv->wMapSvrNum,aRecv->szGuildName);
	if (bRet == TRUE )
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
			pResult.iRegMarkCount = sManager->db.GetAsInteger("REG_MARKS");
		}
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsGuildMarkReset(cDataServer * sManager, int aIndex, CSP_REQ_GUILDRESETMARK * aRecv) 
{
	//OutputDebugString("GS_DGAnsGuildMarkReset\n");

	CSP_ANS_GUILDRESETMARK pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x11, sizeof(pResult));
	
	pResult.iIndex = aRecv->iIndex;
	pResult.iRegMarkCount = 0;
	pResult.iResult = 0;
	//OutputDebugString("memcpy(pResult.szGuildName,aRecv->szGuildName,8);\n");
	memcpy(pResult.szGuildName,aRecv->szGuildName,8);
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ModifyGuildMarkReset %d, '%s'",aRecv->wMapSvrNum,aRecv->szGuildName);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
			pResult.iRegMarkCount = sManager->db.GetAsInteger("DEL_MARKS");
		}
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsGuildSetGiveUp(cDataServer * sManager, int aIndex, CSP_REQ_GUILDSETGIVEUP * aRecv) 
{
	//OutputDebugString("GS_DGAnsGuildSetGiveUp\n");

	CSP_ANS_GUILDSETGIVEUP pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x12, sizeof(pResult));
	
	pResult.bIsGiveUp = aRecv->bIsGiveUp;
	pResult.iIndex = aRecv->iIndex;
	pResult.iRegMarkCount = 0;
	pResult.iResult = 0;
	//OutputDebugString("memcpy(pResult.szGuildName,aRecv->szGuildName,8);\n");
	memcpy(pResult.szGuildName,aRecv->szGuildName,8);
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ModifyGuildGiveUp %d, '%s', %d",aRecv->wMapSvrNum,aRecv->szGuildName,aRecv->bIsGiveUp);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
			pResult.iRegMarkCount = sManager->db.GetAsInteger("DEL_MARKS");
		}
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsNpcRemove(cDataServer * sManager, int aIndex, CSP_REQ_NPCREMOVE * aRecv) 
{
	//OutputDebugString("GS_DGAnsNpcRemove\n");

	CSP_ANS_NPCREMOVE pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x16, sizeof(pResult));
	
	pResult.iNpcIndex = aRecv->iNpcIndex;
	pResult.iNpcNumber = aRecv->iNpcNumber;
	pResult.iResult = 0;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ReqNpcRemove %d, %d, %d",aRecv->wMapSvrNum,aRecv->iNpcNumber,aRecv->iNpcIndex);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsCastleStateSync(cDataServer * sManager, int aIndex, CSP_REQ_CASTLESTATESYNC * aRecv) 
{
	////OutputDebugString("GS_DGAnsCastleStateSync\n");

	CSP_ANS_CASTLESTATESYNC pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x17, sizeof(pResult));
	
	pResult.iCastleState = aRecv->iCastleState;
	pResult.iTaxHuntZone = aRecv->iTaxHuntZone;
	pResult.iTaxRateChaos = aRecv->iTaxRateChaos;
	pResult.iTaxRateStore = aRecv->iTaxRateStore;
	//OutputDebugString("memcpy(pResult.szOwnerGuildName,aRecv->szOwnerGuildName,8);\n");
	memcpy(pResult.szOwnerGuildName,aRecv->szOwnerGuildName,8);
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	sManager->DataSendAll((unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsCastleTributeMoney(cDataServer * sManager, int aIndex, CSP_REQ_CASTLETRIBUTEMONEY * aRecv) 
{
	//OutputDebugString("GS_DGAnsCastleTributeMoney\n");

	CSP_ANS_CASTLETRIBUTEMONEY pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x18, sizeof(pResult));
	
	pResult.iResult = 0;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ModifyMoney %d, %d",aRecv->wMapSvrNum,aRecv->iCastleTributeMoney);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsResetCastleTaxInfo(cDataServer * sManager, int aIndex, CSP_REQ_RESETCASTLETAXINFO * aRecv) 
{
	//OutputDebugString("GS_DGAnsResetCastleTaxInfo\n");

	CSP_ANS_RESETCASTLETAXINFO pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x19, sizeof(pResult));
	
	pResult.iResult = 0;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ResetCastleTaxInfo %d",aRecv->wMapSvrNum);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsResetSiegeGuildInfo(cDataServer * sManager, int aIndex, CSP_REQ_RESETSIEGEGUILDINFO * aRecv) 
{
	//OutputDebugString("GS_DGAnsResetSiegeGuildInfo\n");

	CSP_ANS_RESETSIEGEGUILDINFO pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x1A, sizeof(pResult));
	
	pResult.iResult = 0;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ResetSiegeGuildInfo %d",aRecv->wMapSvrNum);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsResetRegSiegeInfo(cDataServer * sManager, int aIndex, CSP_REQ_RESETREGSIEGEINFO * aRecv) 
{
	//OutputDebugString("GS_DGAnsResetRegSiegeInfo\n");

	CSP_ANS_RESETREGSIEGEINFO pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0x80,0x1B, sizeof(pResult));
	
	pResult.iResult = 0;
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_ResetRegSiegeInfo %d",aRecv->wMapSvrNum);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.iResult = sManager->db.GetAsInteger("QueryResult");
		}
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------

void GS_DGAnsCastleInitData(cDataServer * sManager, int aIndex, CSP_REQ_CSINITDATA * aRecv) 
{
	//OutputDebugString("GS_DGAnsCastleInitData\n");

	BOOL bRet = FALSE;
	int Counter = 0;
	char cBUFFER[10000]={0};

	CSP_ANS_CSINITDATA * pResult = (CSP_ANS_CSINITDATA *)(cBUFFER);
	CSP_CSINITDATA * NPCInfo = (CSP_CSINITDATA *)(cBUFFER + sizeof(CSP_ANS_CSINITDATA));
	
	pResult->btEndDay = 0;
	pResult->btEndMonth = 0;
	pResult->btIsCastleOccupied = 0;
	pResult->btIsSiegeEnded = 0;
	pResult->btIsSiegeGuildList = 0;
	pResult->btStartDay = 0;
	pResult->btStartMonth = 0;
	pResult->i64CastleMoney = 0;
	pResult->iCount = 0;
	pResult->iFirstCreate = 0;
	pResult->iResult  = 0;
	pResult->iTaxHuntZone = 0;
	pResult->iTaxRateChaos = 0;
	pResult->iTaxRateStore = 0;
	memset(pResult->szCastleOwnGuild,0,8);
	//memcpy(pResult->szCastleOwnGuild,0,8);
	pResult->wEndYear = 0;
	pResult->wMapSvrNum = aRecv->wMapSvrNum;
	pResult->wStartYear = 0;
	
	int lOfs=sizeof(CSP_ANS_CSINITDATA);

	//Open Call 1
	bRet = sManager->db.ExecQuery("EXEC WZ_CS_GetCastleTotalInfo %d,%d",aRecv->wMapSvrNum,aRecv->iCastleEventCycle);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult->iResult = 1;
			pResult->btEndDay = sManager->db.GetAsInteger("EDAY");
			pResult->btEndMonth = sManager->db.GetAsInteger("EMONTH");
			pResult->btIsCastleOccupied = sManager->db.GetAsInteger("CASLE_OCCUPY");
			pResult->btIsSiegeEnded = sManager->db.GetAsInteger("SIEGE_ENDED");
			pResult->btIsSiegeGuildList = sManager->db.GetAsInteger("SIEGE_GUILDLIST_SETTED");
			pResult->btStartDay = sManager->db.GetAsInteger("SDAY");
			pResult->btStartMonth = sManager->db.GetAsInteger("SMONTH");
			pResult->i64CastleMoney = sManager->db.GetAsInteger64("MONEY");
			pResult->iFirstCreate = sManager->db.GetAsInteger("FIRST_CREATE");
			pResult->iTaxHuntZone = sManager->db.GetAsInteger("TAX_RATE_ZONE");
			pResult->iTaxRateChaos = sManager->db.GetAsInteger("TAX_RATE_CHAOS");
			pResult->iTaxRateStore = sManager->db.GetAsInteger("TAX_RATE_STORE");
			sManager->db.GetAsString("OWNER_GUILD", pResult->szCastleOwnGuild);
			pResult->wEndYear = sManager->db.GetAsInteger("EYEAR");
			pResult->wStartYear = sManager->db.GetAsInteger("SYEAR");
			sManager->db.Close();	//Close Call 1

			//Open Call 2
			bRet = sManager->db.ExecQuery("EXEC WZ_CS_GetCastleNpcInfo %d",aRecv->wMapSvrNum);
			if (bRet == TRUE )
			{
				if( sManager->db.Fetch() != SQL_NO_DATA)
				{
					do
					{
						NPCInfo = (CSP_CSINITDATA *)(cBUFFER + lOfs);
						NPCInfo->iNpcNumber = sManager->db.GetAsInteger("NPC_NUMBER");
						NPCInfo->iNpcIndex = sManager->db.GetAsInteger("NPC_INDEX");
						NPCInfo->iNpcDfLevel = sManager->db.GetAsInteger("NPC_DF_LEVEL");
						NPCInfo->iNpcRgLevel = sManager->db.GetAsInteger("NPC_RG_LEVEL");
						NPCInfo->iNpcMaxHp = sManager->db.GetAsInteger("NPC_MAXHP");
						NPCInfo->iNpcHp = sManager->db.GetAsInteger("NPC_HP");
						NPCInfo->btNpcX = sManager->db.GetAsInteger("NPC_X");
						NPCInfo->btNpcY = sManager->db.GetAsInteger("NPC_Y");
						NPCInfo->btNpcDIR = sManager->db.GetAsInteger("NPC_DIR");
						lOfs+=sizeof(CSP_CSINITDATA);
						Counter++;
					}while(Counter < 200 && sManager->db.Fetch() != SQL_NO_DATA);
				}
			}
			sManager->db.Close();	//Close Call 2
		} else {
			sManager->db.Close();	//Close Call 1
		}
	} else {
		sManager->db.Close();	//Close Call 1
	}

	PHeadSetW((LPBYTE)pResult, 0x81, sizeof(CSP_ANS_CSINITDATA) + sizeof(CSP_CSINITDATA) * Counter);
	pResult->iCount = Counter;
	sManager->DataSend(aIndex, (unsigned char*)cBUFFER, sizeof(CSP_ANS_CSINITDATA) + sizeof(CSP_CSINITDATA) * Counter);
}
//---------------------------------------------------------------------------

void GS_DGAnsAllGuildMarkRegInfo(cDataServer * sManager, int aIndex, CSP_REQ_ALLGUILDREGINFO * aRecv) 
{
	//OutputDebugString("GS_DGAnsAllGuildMarkRegInfo\n");

	char cBUFFER[10000]={0};

	CSP_ANS_ALLGUILDREGINFO * pResult = (CSP_ANS_ALLGUILDREGINFO *)(cBUFFER);
	CSP_GUILDREGINFO * GuildInfo = (CSP_GUILDREGINFO *)(cBUFFER + sizeof(CSP_ANS_ALLGUILDREGINFO));
	int Counter = 0;

	pResult->iIndex = aRecv->iIndex;
	pResult->iResult = 1;
	pResult->wMapSvrNum = aRecv->wMapSvrNum;
	GuildInfo->iRegMarkCount = 0;
	GuildInfo->btRegRank = 0;
	GuildInfo->bIsGiveUp = 0;
	memset(GuildInfo->szGuildName,0,8);
	//memcpy(GuildInfo->szGuildName,0,8);
	
	int lOfs=sizeof(CSP_ANS_ALLGUILDREGINFO);

	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_GetAllGuildMarkRegInfo %d",aRecv->wMapSvrNum);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{
			do
			{
				GuildInfo = (CSP_GUILDREGINFO *)(cBUFFER + lOfs);
				char tmp[11]={0};
				GuildInfo->bIsGiveUp = sManager->db.GetAsInteger("IS_GUIVEUP");
				GuildInfo->btRegRank = (BYTE) sManager->db.GetAsInteger("SEQ_NUM");
				GuildInfo->iRegMarkCount = sManager->db.GetAsInteger("REG_MARKS");
				sManager->db.GetAsString("REG_SIEGE_GUILD", tmp);
				//OutputDebugString("memcpy(GuildInfo->szGuildName,tmp,8);\n");
				memcpy(GuildInfo->szGuildName,tmp,8);
				Counter++;
				lOfs+=sizeof(CSP_GUILDREGINFO);
			}while(Counter < 100 && sManager->db.Fetch() != SQL_NO_DATA);
		}
	}
	sManager->db.Close();

	PHeadSetW((LPBYTE)pResult, 0x83, sizeof(CSP_ANS_ALLGUILDREGINFO) + sizeof(CSP_GUILDREGINFO) * Counter);
	pResult->iCount = Counter;
	sManager->DataSend(aIndex, (unsigned char*)cBUFFER, sizeof(CSP_ANS_ALLGUILDREGINFO) + sizeof(CSP_GUILDREGINFO) * Counter);
}
//---------------------------------------------------------------------------

void GS_DGAnsFirstCreateNPC(cDataServer * sManager, int aIndex, CSP_REQ_NPCSAVEDATA * aRecv) 
{
	//OutputDebugString("GS_DGAnsFirstCreateNPC\n");

	int Counter = 0;
	BOOL bRet = FALSE;
	CSP_ANS_NPCSAVEDATA pResult;
	pResult.h.set(0x84, sizeof(pResult));
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	pResult.iResult = 0;

	int lOfs=sizeof(CSP_REQ_NPCSAVEDATA);

	//Open Call 1
	bRet = sManager->db.ExecQuery("DELETE FROM MuCastle_NPC WHERE MAP_SVR_GROUP = %d",aRecv->wMapSvrNum);
	sManager->db.Close();	//Close Call 1

	if (bRet == TRUE )
	{
		if(aRecv->iCount > 0)
		{
			while(Counter<aRecv->iCount)
			{
				CSP_NPCSAVEDATA * NPCInfo = (CSP_NPCSAVEDATA*)((LPBYTE)aRecv + lOfs);

				//Open Call 2
				bRet = sManager->db.ExecQuery("EXEC WZ_CS_ReqNpcBuy %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",aRecv->wMapSvrNum,NPCInfo->iNpcNumber,NPCInfo->iNpcIndex,NPCInfo->iNpcDfLevel,NPCInfo->iNpcRgLevel,NPCInfo->iNpcMaxHp,NPCInfo->iNpcHp,NPCInfo->btNpcX,NPCInfo->btNpcY,NPCInfo->btNpcDIR);
				if (bRet != TRUE )
				{
					sManager->db.Close();	//Close Call 2
					break;
				}

				Counter++;
				lOfs+=sizeof(CSP_NPCSAVEDATA);
				sManager->db.Close();	//Close Call 2
			}
			pResult.iResult = 1;
		}
	}

	sManager->DataSend(aIndex, (unsigned char*)&pResult,sizeof(pResult));
}
//---------------------------------------------------------------------------

void GS_DGAnsCalcRegGuildList(cDataServer * sManager, int aIndex, CSP_REQ_CALCREGGUILDLIST * aRecv) 
{
/*
	BYTE SendData[sizeof(CSP_ANS_CALCREGGUILDLIST) + sizeof(CSP_CALCREGGUILDLIST) * 100] = {0};
	
	CSP_ANS_CALCREGGUILDLIST * lpMsg = (CSP_ANS_CALCREGGUILDLIST *)SendData;
	CSP_CALCREGGUILDLIST * lpMsgBody = (CSP_CALCREGGUILDLIST *)&SendData[sizeof(CSP_ANS_CALCREGGUILDLIST)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x85;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iResult = 0;

	int iCount = 0;

	if(sManager->db.ExecQuery("EXEC WZ_CS_GetCalcRegGuildList %d",aRecv->wMapSvrNum) == TRUE )
	{
		lpMsg->iResult = TRUE;

		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			do
			{
				sManager->db.GetAsString("REG_SIEGE_GUILD",lpMsgBody[iCount].szGuildName,sizeof(lpMsgBody->szGuildName));
				lpMsgBody[iCount].iRegMarkCount = sManager->db.GetAsInteger("REG_MARKS");
				lpMsgBody[iCount].iGuildMemberCount = sManager->db.GetAsInteger("GUILD_MEMBER");
				lpMsgBody[iCount].iGuildMasterLevel = sManager->db.GetAsInteger("GM_LEVEL");
				lpMsgBody[iCount].iSeqNum = sManager->db.GetAsInteger("SEQ_NUM");
			}
			while((iCount++) < 100 && sManager->db.Fetch() != SQL_NO_DATA);
		}
	}

	int size = sizeof(CSP_ANS_CALCREGGUILDLIST) + sizeof(CSP_CALCREGGUILDLIST) * iCount;

	lpMsg->iCount = iCount;
	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	sManager->db.Close();
	sManager->DataSend(aIndex,(PBYTE)SendData,size);
	*/
	
	//OutputDebugString("GS_DGAnsCalcRegGuildList\n");

	char cBUFFER[10000]={0};

	CSP_ANS_CALCREGGUILDLIST * pResult = (CSP_ANS_CALCREGGUILDLIST *)(cBUFFER);
	CSP_CALCREGGUILDLIST * GuildInfo = (CSP_CALCREGGUILDLIST *)(cBUFFER + sizeof(CSP_ANS_CALCREGGUILDLIST));
	int Counter = 0;

	pResult->iResult = 1;
	pResult->wMapSvrNum = aRecv->wMapSvrNum;
	GuildInfo->iGuildMasterLevel = 0;
	GuildInfo->iGuildMemberCount = 0;
	GuildInfo->iRegMarkCount = 0;
	GuildInfo->iSeqNum = 0;
	memset(GuildInfo->szGuildName,0,8);
	//memcpy(GuildInfo->szGuildName,0,8);
	
	int lOfs=sizeof(CSP_ANS_CALCREGGUILDLIST);

	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_GetCalcRegGuildList %d",aRecv->wMapSvrNum);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{
			do
			{
				GuildInfo = (CSP_CALCREGGUILDLIST *)(cBUFFER + lOfs);
				char tmp[11]={0};
				GuildInfo->iGuildMasterLevel = sManager->db.GetAsInteger("GM_LEVEL");
				GuildInfo->iGuildMemberCount = sManager->db.GetAsInteger("GUILD_MEMBER");
				GuildInfo->iRegMarkCount = sManager->db.GetAsInteger("REG_MARKS");
				GuildInfo->iSeqNum = sManager->db.GetAsInteger("SEQ_NUM");
				sManager->db.GetAsString("REG_SIEGE_GUILD", tmp);
				//OutputDebugString("memcpy(GuildInfo->szGuildName,tmp,8);\n");
				memcpy(GuildInfo->szGuildName,tmp,8);
				Counter++;
				lOfs+=sizeof(CSP_CALCREGGUILDLIST);
			}while(Counter < 100 && sManager->db.Fetch() != SQL_NO_DATA);
		}
	}
	sManager->db.Close();

	PHeadSetW((LPBYTE)pResult, 0x85, sizeof(CSP_ANS_CALCREGGUILDLIST) + sizeof(CSP_CALCREGGUILDLIST) * Counter);
	pResult->iCount = Counter;
	sManager->DataSend(aIndex, (unsigned char*)cBUFFER, sizeof(CSP_ANS_CALCREGGUILDLIST) + sizeof(CSP_CALCREGGUILDLIST) * Counter);
	
}
//---------------------------------------------------------------------------

void GS_DGAnsCsGulidUnionInfo(cDataServer * sManager, int aIndex, CSP_REQ_CSGUILDUNIONINFO * aRecv) 
{

	BYTE SendData[sizeof(CSP_ANS_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * 100 * 3] = {0};

	CSP_CSGUILDUNIONINFO * aRecvBody = (CSP_CSGUILDUNIONINFO*)((PBYTE)aRecv + sizeof(CSP_REQ_CSGUILDUNIONINFO));

	CSP_ANS_CSGUILDUNIONINFO * lpMsg = (CSP_ANS_CSGUILDUNIONINFO*)SendData;
	CSP_CSGUILDUNIONINFO * lpMsgBody = (CSP_CSGUILDUNIONINFO*)&SendData[sizeof(CSP_ANS_CSGUILDUNIONINFO)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x86;
	lpMsg->iResult = TRUE;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;

	int iCount = 0;

	for(int i = 0; i < aRecv->iCount; i++)
	{
		char szGuildName[9] = {0};
		memcpy(szGuildName,aRecvBody[i].szGuildName,8);

		if(sManager->db.ExecQuery("EXEC WZ_CS_GetCsGuildUnionInfo '%s'",szGuildName) == TRUE && sManager->db.Fetch() != SQL_NO_DATA)
		{
			do
			{
				sManager->db.GetAsString("GUILD_NAME",lpMsgBody[iCount].szGuildName,sizeof(lpMsgBody->szGuildName));
				lpMsgBody[iCount].iCsGuildID = aRecvBody[i].iCsGuildID;
			}
			while((iCount++) < 100 && sManager->db.Fetch() != SQL_NO_DATA);
		}
		else
		{
			lpMsg->iResult = 0;
			break;
		}
		sManager->db.Close();
	}

	int size = sizeof(CSP_ANS_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * iCount;

	lpMsg->iCount = iCount;

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	sManager->DataSend(aIndex,(PBYTE)SendData,size);
	/*
	//OutputDebugString("GS_DGAnsCsGulidUnionInfo\n");

	int Counter = 0;
	int GuildCount = 0;
	char cBUFFER[10000]={0};

	CSP_ANS_CSGUILDUNIONINFO * pResult = (CSP_ANS_CSGUILDUNIONINFO *)(cBUFFER);
	CSP_CSGUILDUNIONINFO * GuildInfo = (CSP_CSGUILDUNIONINFO *)(cBUFFER + sizeof(CSP_ANS_CSGUILDUNIONINFO));

	//PHeadSetB((LPBYTE)pResult, 0x86, sizeof(pResult));
	pResult->h.set(0x86, sizeof(pResult));
	pResult->wMapSvrNum = aRecv->wMapSvrNum;
	pResult->iResult = 0;
	pResult->iCount = 0;

	int lOfs=sizeof(CSP_REQ_CSSAVETOTALGUILDINFO);
	int ROfs=sizeof(CSP_ANS_CSGUILDUNIONINFO);

	if(aRecv->iCount > 0)
	{
		while(Counter<aRecv->iCount)
		{
			CSP_CSGUILDUNIONINFO * GuildRecvInfo = (CSP_CSGUILDUNIONINFO*)((LPBYTE)aRecv + lOfs);
			BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_GetCsGuildUnionInfo '%s'",GuildRecvInfo->szGuildName);
			
			if(( sManager->db.Fetch() != SQL_NO_DATA) && (bRet != TRUE ))
			{
				sManager->db.Close();
				break;
			}
			
			do
			{
				GuildInfo = (CSP_CSGUILDUNIONINFO *)(cBUFFER + ROfs);
				char tmp[11]={0};
				sManager->db.GetAsString("GUILD_NAME", tmp);
				//OutputDebugString("memcpy(GuildInfo->szGuildName,tmp,8);\n");
				memcpy(GuildInfo->szGuildName,tmp,8);
				GuildInfo->iCsGuildID = GuildCount;
				GuildCount++;
				ROfs+=sizeof(CSP_CSGUILDUNIONINFO);
			}while(GuildCount < 100 && sManager->db.Fetch() != SQL_NO_DATA);

			sManager->db.Close();

			Counter++;
			lOfs+=sizeof(CSP_CSGUILDUNIONINFO);
		}
		pResult->iResult = 1;
	}

	PHeadSetW((LPBYTE)pResult, 0x86, sizeof(CSP_ANS_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * GuildCount);
	pResult->iCount = Counter;
	sManager->DataSend(aIndex, (unsigned char*)cBUFFER, sizeof(CSP_ANS_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * GuildCount);
	*/
}
//---------------------------------------------------------------------------

void GS_DGAnsCsSaveTotalGuildInfo(cDataServer * sManager, int aIndex, CSP_REQ_CSSAVETOTALGUILDINFO * aRecv) 
{
	sManager->db.ExecQuery("DELETE MuCastle_SIEGE_GUILDLIST WHERE MAP_SVR_GROUP = %d",aRecv->wMapSvrNum);
	sManager->db.Close();

	CSP_ANS_CSSAVETOTALGUILDINFO pMsg;
	CSP_CSSAVETOTALGUILDINFO * lpMsgBody = (CSP_CSSAVETOTALGUILDINFO*)((PBYTE)aRecv+sizeof(CSP_REQ_CSSAVETOTALGUILDINFO));

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x87;

	pMsg.iResult = TRUE;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	for(int i = 0; i < aRecv->iCount;i++)
	{
		char szGuildName[9] = {0};
		memcpy(szGuildName,lpMsgBody[i].szGuildName,8);

		if(sManager->db.ExecQuery("EXEC WZ_CS_SetSiegeGuildInfo %d, '%s', %d, %d",
			aRecv->wMapSvrNum,szGuildName,lpMsgBody[i].iCsGuildID,lpMsgBody[i].iGuildInvolved) == TRUE)
		{

		}
		else
		{
			pMsg.iResult = FALSE;
			break;
		}

		sManager->db.Close();
	}
	sManager->DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg));
	/*
	//OutputDebugString("GS_DGAnsCsSaveTotalGuildInfo\n");

	int Counter = 0;
	CSP_ANS_CSSAVETOTALGUILDINFO pResult;
	pResult.h.set(0x87, sizeof(pResult));
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	pResult.iResult = 0;

	int lOfs=sizeof(CSP_REQ_CSSAVETOTALGUILDINFO);

	if(aRecv->iCount > 0)
	{
		while(Counter<aRecv->iCount)
		{
			CSP_CSSAVETOTALGUILDINFO * GuildInfo = (CSP_CSSAVETOTALGUILDINFO*)((LPBYTE)aRecv + lOfs);
			BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_SetSiegeGuildInfo %d, '%s', %d, %d",aRecv->wMapSvrNum,GuildInfo->szGuildName,GuildInfo->iCsGuildID,GuildInfo->iGuildInvolved);
			if (bRet != TRUE )
			{
				sManager->db.Close();
				break;
			}
			sManager->db.Close();
			Counter++;
			lOfs+=sizeof(CSP_CSSAVETOTALGUILDINFO);
		}
		pResult.iResult = 1;
	}
	sManager->DataSend(aIndex, (unsigned char*)&pResult,sizeof(pResult));
	*/
}
//---------------------------------------------------------------------------

void GS_DGAnsCsLoadTotalGuildInfo(cDataServer * sManager, int aIndex, CSP_REQ_CSLOADTOTALGUILDINFO * aRecv) 
{	
	BYTE SendData[sizeof(CSP_ANS_CSLOADTOTALGUILDINFO) + sizeof(CSP_CSLOADTOTALGUILDINFO) * 100] = {0};

	CSP_ANS_CSLOADTOTALGUILDINFO * lpMsg = (CSP_ANS_CSLOADTOTALGUILDINFO*)SendData;
	CSP_CSLOADTOTALGUILDINFO * lpMsgBody = (CSP_CSLOADTOTALGUILDINFO*)&SendData[sizeof(CSP_ANS_CSLOADTOTALGUILDINFO)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x88;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iResult = FALSE;

	int iCount = 0;

	if(sManager->db.ExecQuery("EXEC WZ_CS_GetSiegeGuildInfo %d",aRecv->wMapSvrNum) == TRUE && sManager->db.Fetch() != SQL_NO_DATA)
	{
		lpMsg->iResult = TRUE;
		do
		{
			sManager->db.GetAsString("GUILD_NAME",lpMsgBody[iCount].szGuildName,sizeof(lpMsgBody->szGuildName));
			lpMsgBody[iCount].iCsGuildID = sManager->db.GetAsInteger("GUILD_ID");
			lpMsgBody[iCount].iGuildInvolved = sManager->db.GetAsInteger("GUILD_INVOLVED");
		}
		while((iCount++) < 100 && sManager->db.Fetch() != SQL_NO_DATA);
	}

	lpMsg->iCount = iCount;

	int size = sizeof(CSP_ANS_CSLOADTOTALGUILDINFO) + sizeof(CSP_CSLOADTOTALGUILDINFO) * iCount;

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	sManager->db.Close();
	sManager->DataSend(aIndex,(PBYTE)SendData,size);
	/*
	//OutputDebugString("GS_DGAnsCsLoadTotalGuildInfo\n");

	char cBUFFER[10000]={0};

	CSP_ANS_CSLOADTOTALGUILDINFO * pResult = (CSP_ANS_CSLOADTOTALGUILDINFO *)(cBUFFER);
	CSP_CSLOADTOTALGUILDINFO * GuildInfo = (CSP_CSLOADTOTALGUILDINFO *)(cBUFFER + sizeof(CSP_ANS_CSLOADTOTALGUILDINFO));
	int Counter = 0;

	pResult->iResult = 1;
	pResult->wMapSvrNum = aRecv->wMapSvrNum;
	GuildInfo->iCsGuildID = 0;
	GuildInfo->iGuildInvolved = 0;
	memset(GuildInfo->szGuildName,0,8);
	//memcpy(GuildInfo->szGuildName,0,8);
	
	int lOfs=sizeof(CSP_ANS_CSLOADTOTALGUILDINFO);

	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CS_GetSiegeGuildInfo %d",aRecv->wMapSvrNum);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{
			do
			{
				GuildInfo = (CSP_CSLOADTOTALGUILDINFO *)(cBUFFER + lOfs);
				char tmp[11]={0};
				GuildInfo->iCsGuildID = sManager->db.GetAsInteger("GUILD_ID");
				GuildInfo->iGuildInvolved = sManager->db.GetAsInteger("GUILD_INVOLVED");
				sManager->db.GetAsString("GUILD_NAME", tmp);
				//OutputDebugString("memcpy(GuildInfo->szGuildName,tmp,8);\n");
				memcpy(GuildInfo->szGuildName,tmp,8);
				Counter++;
				lOfs+=sizeof(CSP_CSLOADTOTALGUILDINFO);
			}while(Counter < 100 && sManager->db.Fetch() != SQL_NO_DATA);
		}
	}
	sManager->db.Close();

	PHeadSetW((LPBYTE)pResult, 0x88, sizeof(CSP_ANS_CSLOADTOTALGUILDINFO) + sizeof(CSP_CSLOADTOTALGUILDINFO) * Counter);
	pResult->iCount = Counter;
	sManager->DataSend(aIndex, (unsigned char*)cBUFFER, sizeof(CSP_ANS_CSLOADTOTALGUILDINFO) + sizeof(CSP_CSLOADTOTALGUILDINFO) * Counter);
	*/
}
//---------------------------------------------------------------------------

void GS_DGAnsCastleNpcUpdate(cDataServer * sManager, int aIndex, CSP_REQ_NPCUPDATEDATA * aRecv) 
{
	//OutputDebugString("GS_DGAnsCastleNpcUpdate\n");

	int Counter = 0;
	CSP_ANS_NPCUPDATEDATA pResult;
	pResult.h.set(0x89, sizeof(pResult));
	pResult.wMapSvrNum = aRecv->wMapSvrNum;
	pResult.iResult = 0;
	BOOL bRet = FALSE;

	int lOfs=sizeof(CSP_REQ_NPCUPDATEDATA);

	bRet = sManager->db.ExecQuery("DELETE FROM MuCastle_NPC WHERE MAP_SVR_GROUP = %d",aRecv->wMapSvrNum);
	sManager->db.Close();

	if (bRet == TRUE )
	{
		if(aRecv->iCount > 0)
		{
			while(Counter<aRecv->iCount)
			{
				CSP_NPCUPDATEDATA * NPCInfo = (CSP_NPCUPDATEDATA*)((LPBYTE)aRecv + lOfs);
				bRet = sManager->db.ExecQuery("EXEC WZ_CS_ReqNpcUpdate %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",aRecv->wMapSvrNum,NPCInfo->iNpcNumber,NPCInfo->iNpcIndex,NPCInfo->iNpcDfLevel,NPCInfo->iNpcRgLevel,NPCInfo->iNpcMaxHp,NPCInfo->iNpcHp,NPCInfo->btNpcX,NPCInfo->btNpcY,NPCInfo->btNpcDIR);
				if (bRet != TRUE )
				{
					sManager->db.Close();
					break;
				}
				Counter++;
				lOfs+=sizeof(CSP_NPCUPDATEDATA);
			}
			pResult.iResult = 1;
		}
	}
	sManager->DataSend(aIndex, (unsigned char*)&pResult,sizeof(pResult));
}
//---------------------------------------------------------------------------

void DGAnsCrywolfSync(cDataServer * sManager, int aIndex, CWP_REQ_CRYWOLFSYNC * aRecv) 
{
	////OutputDebugString("DGAnsCrywolfSync\n");

	CWP_ANS_CRYWOLFSYNC pResult;
	pResult.h.set(0xB0, sizeof(pResult));
	pResult.iCrywolfState = aRecv->iCrywolfState;
	pResult.iOccupationState = aRecv->iOccupationState;
	pResult.btResult = aRecv->iOccupationState;	//REVEER

	sManager->DataSendAll((unsigned char*)&pResult,sizeof(pResult));
}
//---------------------------------------------------------------------------

void DGAnsCrywolfInfoLoad(cDataServer * sManager, int aIndex, CWP_REQ_CRYWOLFINFOLOAD * aRecv) 
{
	//OutputDebugString("DGAnsCrywolfInfoLoad\n");

	CWP_ANS_CRYWOLFINFOLOAD pResult;
	pResult.h.set(0xB1, sizeof(pResult));
	
	pResult.btResult = 0;
	pResult.iCrywolfState = 0;
	pResult.iOccupationState = 0;
	
	BOOL bRet = sManager->db.ExecQuery("EXEC WZ_CW_InfoLoad %d",aRecv->wMapSvrNum);
	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{	
			pResult.btResult = 1;
			pResult.iCrywolfState = sManager->db.GetAsInteger("CRYWOLF_STATE");
			pResult.iOccupationState = sManager->db.GetAsInteger("CRYWOLF_OCCUFY");
		}
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (unsigned char*)&pResult, pResult.h.size);
}
//---------------------------------------------------------------------------