#include "stdafx.h"
#include "config.h"
#include "winutil.h"
#include "SCFExDBProtocol.h"
#include "rprotocol.h"
#include "eprotocol.h"
#include "../stdafx.h"
#include "../SCFExDB_Defines.h"
#include "Log.h"



void GetRewardInfo(cDataServer * sManager, SDHP_GETREWARD_INFOSAVE * lpMsg)
{
	char szAccountID[11]			= {0};
	char szName[11]					= {0};

	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->Name, 10);

	
	if((SQLSyntexCheck(szName) == TRUE)	&&
		(SpaceSyntexCheck(szName) == TRUE))
	{
		if((SQLSyntexCheck(szAccountID) == TRUE)	&&
			(SpaceSyntexCheck(szAccountID) == TRUE))
		{
			sManager->db.ExecQuery("INSERT INTO %s.dbo.Titan_Rewards (AccountID,Name,Zen,VipMoney,Num,Lvl,Opt,Luck,Skill,Dur,Excellent,Ancient,JOH,Sock1,Sock2,Sock3,Sock4,Sock5,Days,SerialFFFFFFFE) VALUES ('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", sManager->m_DBName,szAccountID,szName,lpMsg->rew.Zen,lpMsg->rew.VipMoney,lpMsg->rew.num,lpMsg->rew.Level,lpMsg->rew.Opt,lpMsg->rew.Luck,lpMsg->rew.Skill,lpMsg->rew.Dur,lpMsg->rew.Exc,lpMsg->rew.Anc,lpMsg->rew.JOH,lpMsg->rew.Sock1,lpMsg->rew.Sock2,lpMsg->rew.Sock3,lpMsg->rew.Sock4,lpMsg->rew.Sock5,lpMsg->rew.Days,lpMsg->rew.IsForeverFFFE);
			sManager->db.Fetch();
			sManager->db.Close();
		}
	}
}


void SetTitanLotteryPlayerBuyTicket(cDataServer * sManager, SDHP_SENDLOTTERYBUYTICKET * aRecv)
{
	char szAccountID[11]			= {0};
	char szName[11]					= {0};

	memcpy(szAccountID, aRecv->AccountID, 10);
	memcpy(szName, aRecv->Name, 10);
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
		(SpaceSyntexCheck(szName) == TRUE))
	{
		if((SQLSyntexCheck(szAccountID) == TRUE)	&&
			(SpaceSyntexCheck(szAccountID) == TRUE))
		{
			sManager->db.ExecQuery("INSERT INTO %s.dbo.TitanLotteryPlayers (AccountID,Name,Num1,Num2,Num3,Num4,Date) VALUES ('%s','%s',%d,%d,%d,%d,GetDate())", sManager->m_DBName,szAccountID,szName,aRecv->Nums[0],aRecv->Nums[1],aRecv->Nums[2],aRecv->Nums[3]);
			sManager->db.Fetch();
			sManager->db.Close();
			
			sManager->db.ExecQuery("UPDATE %s.dbo.TitanLotteryReward SET Reward = Reward + %d WHERE ID=2",sManager->m_DBName,aRecv->ToReward);
			sManager->db.Fetch();
			sManager->db.Close();
		}
	}	
}

void SetTitanLotteryInfo(cDataServer * sManager, SDHP_SENDLOTTERYINFO * aRecv)
{
	int Cnt = 0;
	int Reward = 0;
	int WinnerCnt = 0;
	int RewardForAll = 0;
	int Zen = 0;
	int VIPMoney = 0;

	BOOL bRet = sManager->db.ExecQuery("SELECT COUNT(*) as cnt FROM %s.dbo.TitanLotteryPlayers WHERE Num1=%d AND Num2=%d AND Num3=%d AND Num4=%d",sManager->m_DBName,aRecv->Nums[0],aRecv->Nums[1],aRecv->Nums[2],aRecv->Nums[3]);

	if(bRet == TRUE)
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			WinnerCnt = sManager->db.GetAsInteger("cnt");
		}
	}
	sManager->db.Close();

	bRet = sManager->db.ExecQuery("SELECT Reward FROM %s.dbo.TitanLotteryReward WHERE ID=1",sManager->m_DBName);
	if(bRet == TRUE)
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			Reward = sManager->db.GetAsInteger("Reward");
		}
	}
	sManager->db.Close();

	int rewTmp = Reward;

	if(WinnerCnt > 0)
	{
		RewardForAll = Reward/WinnerCnt;

		if(RewardForAll > aRecv->info.MaxReward)
			RewardForAll = aRecv->info.MaxReward;

		if(RewardForAll < aRecv->info.MinReward)
			RewardForAll = aRecv->info.MinReward;

		rewTmp = RewardForAll;
	}

	sManager->db.ExecQuery("INSERT INTO %s.dbo.TitanLotteryLog (Num1,Num2,Num3,Num4,Date,Reward,Winners) VALUES (%d,%d,%d,%d,GetDate(),%d,%d)",sManager->m_DBName,aRecv->Nums[0],aRecv->Nums[1],aRecv->Nums[2],aRecv->Nums[3],rewTmp,WinnerCnt);
	sManager->db.Fetch();
	sManager->db.Close();

	if(WinnerCnt > 0)
	{
		sManager->db.ExecQuery("INSERT INTO %s.dbo.TitanLotteryWinners (AccountID,Name,Date) SELECT AccountID, Name, GetDate() FROM %s.dbo.TitanLotteryPlayers WHERE Num1=%d AND Num2=%d AND Num3=%d AND Num4=%d",
			sManager->m_DBName,sManager->m_DBName,aRecv->Nums[0],aRecv->Nums[1],aRecv->Nums[2],aRecv->Nums[3]);
		sManager->db.Fetch();
		sManager->db.Close();


		if(aRecv->info.Type == 0)		//Zen
		{
			Zen = RewardForAll;
		}else if(aRecv->info.Type == 1)	//VIPMoney
		{
			VIPMoney = RewardForAll;
		}

		bool GetReward = false;

		if(aRecv->info.AdditionalReward > 0) //Extra Reward
		{
			if(WinnerCnt <= aRecv->info.AdditionalReward)
			{
				sManager->db.ExecQuery("INSERT INTO %s.dbo.Titan_Rewards (AccountID,Name,Zen,VipMoney,Num,Lvl,Opt,Luck,Skill,Dur,Excellent,Ancient,JOH,Sock1,Sock2,Sock3,Sock4,Sock5,Days,SerialFFFFFFFE) SELECT AccountID,Name,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d FROM %s.dbo.TitanLotteryPlayers WHERE Num1=%d AND Num2=%d AND Num3=%d AND Num4=%d",
					sManager->m_DBName,(Zen+aRecv->ExReward.Zen),(VIPMoney+aRecv->ExReward.VipMoney),aRecv->ExReward.num,aRecv->ExReward.Level,aRecv->ExReward.Opt,aRecv->ExReward.Luck,aRecv->ExReward.Skill,aRecv->ExReward.Dur,aRecv->ExReward.Exc,aRecv->ExReward.Anc,aRecv->ExReward.JOH,aRecv->ExReward.Sock1,aRecv->ExReward.Sock2,aRecv->ExReward.Sock3,aRecv->ExReward.Sock4,aRecv->ExReward.Sock5,aRecv->ExReward.Days,aRecv->ExReward.IsForeverFFFE,sManager->m_DBName,aRecv->Nums[0],aRecv->Nums[1],aRecv->Nums[2],aRecv->Nums[3]);
				sManager->db.Fetch();
				sManager->db.Close();

				GetReward = true;
			}
		}

		if(GetReward == false)
		{
			//Save Reward
			sManager->db.ExecQuery("INSERT INTO %s.dbo.Titan_Rewards (AccountID,Name,Zen,VipMoney,Num,Lvl,Opt,Luck,Skill,Dur,Excellent,Ancient,JOH,Sock1,Sock2,Sock3,Sock4,Sock5,Days,SerialFFFFFFFE) SELECT AccountID,Name,%d,%d,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 FROM %s.dbo.TitanLotteryPlayers WHERE Num1=%d AND Num2=%d AND Num3=%d AND Num4=%d",
				sManager->m_DBName,Zen,VIPMoney,sManager->m_DBName,aRecv->Nums[0],aRecv->Nums[1],aRecv->Nums[2],aRecv->Nums[3]);
			sManager->db.Fetch();
			sManager->db.Close();
		}

		//Mover de 2 a 1
		sManager->db.ExecQuery("UPDATE %s.dbo.TitanLotteryReward SET Reward = (SELECT DISTINCT TOP 1 Reward FROM %s.dbo.TitanLotteryReward WHERE ID=2) WHERE ID=1",
			sManager->m_DBName,sManager->m_DBName);
		sManager->db.Fetch();
		sManager->db.Close();
	}else
	{
		//Mover de 2 a 1
		sManager->db.ExecQuery("UPDATE %s.dbo.TitanLotteryReward SET Reward = Reward + (SELECT DISTINCT TOP 1 Reward FROM %s.dbo.TitanLotteryReward WHERE ID=2) WHERE ID=1",
			sManager->m_DBName,sManager->m_DBName);
		sManager->db.Fetch();
		sManager->db.Close();
	}

	//SET Ammount = 0
	sManager->db.ExecQuery("UPDATE %s.dbo.TitanLotteryReward SET Reward = 0 WHERE ID=2",
		sManager->m_DBName);
	sManager->db.Fetch();
	sManager->db.Close();
	
	sManager->db.ExecQuery("DELETE FROM %s.dbo.TitanLotteryPlayers",
		sManager->m_DBName);
	sManager->db.Fetch();
	sManager->db.Close();

	int NextReward = 0;
	bRet = sManager->db.ExecQuery("SELECT Reward FROM %s.dbo.TitanLotteryReward WHERE ID=1",sManager->m_DBName);
	if(bRet == TRUE)
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			NextReward = sManager->db.GetAsInteger("Reward");
		}
	}
	sManager->db.Close();
	
	SDHP_RECVLOTTERYINFO result = {0};	
	PHeadSetB((LPBYTE)&result, SCFExDB_GSRecv_GetTitanLotteryInfo, sizeof(result));
	result.WinnersCount = WinnerCnt;
	result.Reward = RewardForAll;
	result.NextReward = NextReward;
	
	sManager->DataSendAll((LPBYTE)&result, sizeof(result));
}

void GetTitanRank(cDataServer * sManager, SDHP_GETRANKINFO * aRecv)
{
	SDHP_GETRANK_INFORECV result = {0};	
	PHeadSetB((LPBYTE)&result, SCFExDB_GSRecv_GetTitanRankInfo, sizeof(result));

	result.rIndex = aRecv->info.rIndex;
	char sOrderBy[100]={0};
	char sWhere[100]={0};
	int Cnt=0;
	
	switch(aRecv->info.Type)
	{
		case 0:
		{
			wsprintf(sOrderBy,"%s DESC,cLevel DESC,SCFMasterLevel DESC",sManager->m_DBResets); 
		}break;
		case 1:
		{
			wsprintf(sOrderBy,"Money DESC,%s DESC,cLevel DESC,SCFMasterLevel DESC",sManager->m_DBResets); 
		}break;
		case 2:
		{
			wsprintf(sOrderBy,"SCFVipMoney DESC,SCFVipDays DESC"); 
		}break;
		case 3:
		{
			wsprintf(sOrderBy,"SCFVipDays DESC,SCFVipMoney DESC"); 
		}break;
		case 4:
		{
			switch(aRecv->info.SubType)
			{
				case 1:
				{
					wsprintf(sOrderBy,"Strength DESC,%s DESC,cLevel DESC,SCFMasterLevel DESC",sManager->m_DBResets); 
				}break;
				case 2:
				{
					wsprintf(sOrderBy,"Dexterity DESC,%s DESC,cLevel DESC,SCFMasterLevel DESC",sManager->m_DBResets); 
				}break;
				case 3:
				{
					wsprintf(sOrderBy,"Vitality DESC,%s DESC,cLevel DESC,SCFMasterLevel DESC",sManager->m_DBResets); 
				}break;
				case 4:
				{
					wsprintf(sOrderBy,"Energy DESC,%s DESC,cLevel DESC,SCFMasterLevel DESC",sManager->m_DBResets); 
				}break;
				case 5:
				{
					wsprintf(sOrderBy,"leadership DESC,%s DESC,cLevel DESC,SCFMasterLevel DESC",sManager->m_DBResets); 
				}break;
				default:
				{
					return;
				}break;
			}
		}break;
		case 5:
		{
			switch(aRecv->info.SubType)
			{
				case 1:
				{
					wsprintf(sOrderBy,"PkCount DESC, PkLevel DESC, PkTime DESC");
				}break;
				case 2:
				{
					wsprintf(sOrderBy,"PkCount ASC, PkLevel ASC, PkTime ASC");
				}break;
				default:
				{
					return;
				}break;
			}
		}break;
		default:
		{
			return;
		}break;
	}
	
	wsprintf(sWhere,"WHERE CtlCode = 0");

	if(aRecv->info.Class >= 0 && aRecv->info.Class != (BYTE) -1)
	{
		BYTE DBClass = (aRecv->info.Class*16);
		wsprintf(sWhere,"%d AND Class BETWEEN %d AND %d",sWhere, DBClass,(DBClass+2));
	}
	
	BOOL bRet = FALSE;
	if(aRecv->info.Type != 2 && aRecv->info.Type != 3)
	{
		bRet = sManager->db.ExecQuery("SELECT TOP %d AccountID,Name FROM %s.dbo.Character %s ORDER BY %s",aRecv->info.Top,sManager->m_DBName,sWhere,sOrderBy);
		
		if(bRet == TRUE)
		{
			while(sManager->db.Fetch() != SQL_NO_DATA && Cnt < aRecv->info.Top)
			{
				sManager->db.GetAsString("AccountID", result.info[Cnt].AccountID);
				sManager->db.GetAsString("Name", result.info[Cnt].Name);
				
				Cnt++;
			}
		}
		sManager->db.Close();
	}else
	{
		if(aRecv->info.Type == 3)
			wsprintf(sWhere,"WHERE SCFIsVip=1");
		else
			wsprintf(sWhere,"");

		bRet = sManager->db.ExecQuery("SELECT TOP %d memb___id FROM %s.dbo.MEMB_INFO %s ORDER BY %s",aRecv->info.Top,sManager->m_DBName2,sWhere,sOrderBy);
		
		if(bRet == TRUE)
		{
			while(sManager->db.Fetch() != SQL_NO_DATA && Cnt < aRecv->info.Top)
			{
				sManager->db.GetAsString("memb___id", result.info[Cnt].AccountID);				
				Cnt++;
			}
			sManager->db.Close();
			
			for(int i=0;i<Cnt;i++)
			{
				BOOL bRet2 = sManager->db.ExecQuery("SELECT GameIDC FROM %s.dbo.AccountCharacter WHERE Id='%s'",sManager->m_DBName,result.info[i].AccountID);
				sManager->db.Fetch();
				if(bRet == TRUE)
				{
					sManager->db.GetAsString("GameIDC", result.info[i].Name);	
				}
				sManager->db.Close();
			}
			
			for(int i=0;i<Cnt;i++)
			{
				if(result.info[Cnt].AccountID != 0 && result.info[i].Name[0] == 0)
				{
					BOOL bRet2 = sManager->db.ExecQuery("SELECT TOP 1 Name FROM %s.dbo.Character WHERE AccountID='%s' ORDER BY %s DESC, cLevel DESC, SCFMasterLevel DESC",sManager->m_DBName,result.info[i].AccountID,sManager->m_DBResets);
					sManager->db.Fetch();
					if(bRet == TRUE)
					{
						sManager->db.GetAsString("Name", result.info[i].Name);	
					}
					sManager->db.Close();
				}
			}
		}
	}	
			
	if(Cnt > 0)
	{
		for(int i=0;i<Cnt;i++)
		{
			if(result.info[Cnt].AccountID != 0)
			{
				sManager->db.ExecQuery("INSERT INTO %s.dbo.Titan_Rank (AccountID,Name,RankDate,RankType,RankSubType,RankTop) VALUES ('%s','%s',getdate(),%d,%d,%d)",
					sManager->m_DBName,result.info[i].AccountID,result.info[i].Name,aRecv->info.Type,aRecv->info.SubType,i+1);
				sManager->db.Fetch();
				sManager->db.Close();
				
				if(i < aRecv->info.RewCnt)
				{
					if(result.info[i].Name[0] != 0)
					{
						sManager->db.ExecQuery("INSERT INTO %s.dbo.Titan_Rewards (AccountID,Name,Zen,VipMoney,Num,Lvl,Opt,Luck,Skill,Dur,Excellent,Ancient,JOH,Sock1,Sock2,Sock3,Sock4,Sock5,Days,SerialFFFFFFFE) VALUES ('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", sManager->m_DBName,result.info[i].AccountID,result.info[i].Name,aRecv->info.Reward[i].Zen,aRecv->info.Reward[i].VipMoney,aRecv->info.Reward[i].num,aRecv->info.Reward[i].Level,aRecv->info.Reward[i].Opt,aRecv->info.Reward[i].Luck,aRecv->info.Reward[i].Skill,aRecv->info.Reward[i].Dur,aRecv->info.Reward[i].Exc,aRecv->info.Reward[i].Anc,aRecv->info.Reward[i].JOH,aRecv->info.Reward[i].Sock1,aRecv->info.Reward[i].Sock2,aRecv->info.Reward[i].Sock3,aRecv->info.Reward[i].Sock4,aRecv->info.Reward[i].Sock5,aRecv->info.Reward[i].Days,aRecv->info.Reward[i].IsForeverFFFE);
						sManager->db.Fetch();
						sManager->db.Close();
					}
				}
			}
		}
		sManager->DataSendAll((LPBYTE)&result, sizeof(result));
	}
}

void GetTitanReward(cDataServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * aRecv)
{
	SDHP_GETREWARD_INFOSAVE result = {0};	
	PHeadSetB((LPBYTE)&result, SCFExDB_GSRecv_GetBotReward, sizeof(result));

	result.aIndex = aRecv->aIndex;
	memcpy(result.AccountID, aRecv->AccountID, 10);
	memcpy(result.Name, aRecv->Name, 10);

	if((SQLSyntexCheck(result.Name) == TRUE)	&&
		(SpaceSyntexCheck(result.Name) == TRUE))
	{
	
		BOOL bRet = sManager->db.ExecQuery("SELECT TOP 1 ID,AccountID,Name,Zen,VipMoney,Num,Lvl,Opt,Luck,Skill,Dur,Excellent,Ancient,JOH,Sock1,Sock2,Sock3,Sock4,Sock5,Days,SerialFFFFFFFE FROM %s.dbo.Titan_Rewards WHERE AccountID='%s' AND Name='%s' ORDER BY ID ASC",sManager->m_DBName,aRecv->AccountID,aRecv->Name);

		int ret = sManager->db.Fetch();
		if ( ret == SQL_NO_DATA)
		{			
			result.ID_Num = -1;	
		}else
		{
			if(bRet == TRUE )
			{
				result.ID_Num				= sManager->db.GetAsInteger("ID");
				result.rew.Zen				= sManager->db.GetAsInteger("Zen");
				result.rew.VipMoney			= sManager->db.GetAsInteger("VipMoney");
				result.rew.num				= sManager->db.GetAsInteger("Num");
				result.rew.Level			= sManager->db.GetAsInteger("Lvl");
				result.rew.Opt				= sManager->db.GetAsInteger("Opt");
				result.rew.Luck				= sManager->db.GetAsInteger("Luck");
				result.rew.Skill			= sManager->db.GetAsInteger("Skill");
				result.rew.Dur				= sManager->db.GetAsInteger("Dur");
				result.rew.Exc				= sManager->db.GetAsInteger("Excellent");
				result.rew.Anc				= sManager->db.GetAsInteger("Ancient");
				result.rew.JOH				= sManager->db.GetAsInteger("JOH");
				result.rew.Sock1			= sManager->db.GetAsInteger("Sock1");
				result.rew.Sock2			= sManager->db.GetAsInteger("Sock2");
				result.rew.Sock3			= sManager->db.GetAsInteger("Sock3");
				result.rew.Sock4			= sManager->db.GetAsInteger("Sock4");
				result.rew.Sock5			= sManager->db.GetAsInteger("Sock5");
				result.rew.Days				= sManager->db.GetAsInteger("Days");
				result.rew.IsForeverFFFE	= sManager->db.GetAsInteger("SerialFFFFFFFE");
			}else			
				result.ID_Num = -1;	
		}

		sManager->db.Close();

		sManager->DataSend(aIndex, (LPBYTE)&result, sizeof(result));
	}
}

void DelTitanReward(cDataServer * sManager, SDHP_GETREWARD_INFODEL * aRecv)
{
	char szAccountID[11]			= {0};
	char szName[11]					= {0};

	memcpy(szAccountID, aRecv->AccountID, 10);
	memcpy(szName, aRecv->Name, 10);
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
		(SpaceSyntexCheck(szName) == TRUE))
	{
		sManager->db.ExecQuery("DELETE FROM %s.dbo.Titan_Rewards WHERE AccountID='%s' AND Name='%s' AND ID=%d", sManager->m_DBName,szAccountID,szName,aRecv->ID_Num);
		sManager->db.Fetch();
		sManager->db.Close();
	}
}

void GJSetRaceRewardInfo(cDataServer * sManager, SDHP_SETRACEREWARD_INFOSAVE * lpMsg)
{	
	sManager->db.ExecQuery("DELETE FROM %s.dbo.Race_LastWinners", sManager->m_DBName);
	sManager->db.Fetch();
	sManager->db.Close();

	if(lpMsg->Count > 0)
	{
		sManager->db.ExecQuery("INSERT INTO %s.dbo.Race_LastWinners (AccountID,Name,Seconds) SELECT TOP %d AccountID, Name, Seconds FROM %s.dbo.Race_Data ORDER BY Seconds ASC",
			sManager->m_DBName,lpMsg->Count,sManager->m_DBName);
		sManager->db.Fetch();
		sManager->db.Close();
	}

	sManager->db.ExecQuery("DELETE FROM %s.dbo.Race_Data", sManager->m_DBName);
	sManager->db.Fetch();
	sManager->db.Close();

	//---	
	
	struct Data_Str
	{
		char szAccountID[11];
		char szName[11];
	};

	Data_Str Data[MAX_BOTRACER_REWARD] = {0};
	int Cnt = 0;

	BOOL bRet = sManager->db.ExecQuery("SELECT TOP %d AccountID, Name, Seconds FROM %s.dbo.Race_LastWinners ORDER BY Seconds ASC",lpMsg->Count,sManager->m_DBName);

	if (bRet == TRUE )
	{
		if( sManager->db.Fetch() != SQL_NO_DATA)
		{
			do
			{
				sManager->db.GetAsString("AccountID", Data[Cnt].szAccountID);
				sManager->db.GetAsString("Name", Data[Cnt].szName);
				Cnt++;
			}while(sManager->db.Fetch() != SQL_NO_DATA);
		}
	}
	sManager->db.Close();

	for(int i=0;i<Cnt;i++)
	{
		sManager->db.ExecQuery("INSERT INTO %s.dbo.Titan_Rewards (AccountID,Name,Zen,VipMoney,Num,Lvl,Opt,Luck,Skill,Dur,Excellent,Ancient,JOH,Sock1,Sock2,Sock3,Sock4,Sock5,Days,SerialFFFFFFFE) VALUES ('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", sManager->m_DBName,Data[i].szAccountID,Data[i].szName,lpMsg->rew[i].Zen,lpMsg->rew[i].VipMoney,lpMsg->rew[i].num,lpMsg->rew[i].Level,lpMsg->rew[i].Opt,lpMsg->rew[i].Luck,lpMsg->rew[i].Skill,lpMsg->rew[i].Dur,lpMsg->rew[i].Exc,lpMsg->rew[i].Anc,lpMsg->rew[i].JOH,lpMsg->rew[i].Sock1,lpMsg->rew[i].Sock2,lpMsg->rew[i].Sock3,lpMsg->rew[i].Sock4,lpMsg->rew[i].Sock5,lpMsg->rew[i].Days,lpMsg->rew[i].IsForeverFFFE);
		sManager->db.Fetch();
		sManager->db.Close();
	}
}

void GJSetPlayerRaceInfo(cDataServer * sManager, int aIndex, SDHP_SETPLAYERRACE_INFOSAVE * lpMsg)
{
	char szName[11]={0};
	memcpy(szName, lpMsg->Name, 10);
	int Seconds = 0;

	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{

		BOOL bRet = sManager->db.ExecQuery("SELECT Name,Seconds FROM %s.dbo.Race_Data WHERE Name = '%s' and AccountID= '%s'",sManager->m_DBName, lpMsg->Name, lpMsg->AccountID);
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			sManager->db.Close();
			sManager->db.ExecQuery("INSERT INTO %s.dbo.Race_Data (AccountID, Name, Seconds) VALUES ('%s','%s',%d)",sManager->m_DBName, lpMsg->AccountID, lpMsg->Name, lpMsg->Seconds);
			sManager->db.Fetch();
			sManager->db.Close();
		}else
		{
			Seconds = sManager->db.GetAsInteger("Seconds"); 
		}
		sManager->db.Close();

		if(Seconds > lpMsg->Seconds)
		{
			sManager->db.ExecQuery("UPDATE %s.dbo.Race_Data SET Seconds=%d WHERE Name='%s' and AccountID='%s'",sManager->m_DBName, lpMsg->Seconds, lpMsg->Name, lpMsg->AccountID);
			sManager->db.Fetch();
			sManager->db.Close();
		}
	}
}

void GJSetPlayerExtraInfo(cDataServer * sManager, int aIndex, SDHP_GETPLAYERINFO_INFOSAVE * lpMsg)
{
	char szName[11]={0};
	char szNameMarry[11]={0};
	memcpy(szName, lpMsg->Name, 10);
	memcpy(szNameMarry, lpMsg->MarriedName, 10);
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{

		if(lpMsg->GensFamily > 0)
		{
			sManager->db.ExecQuery("UPDATE %s.dbo.Character SET SCFMasterLevel=%d, SCFMasterPoints=%d, %s=%d, SCFPCPoints=%d, SCFSealItem=%d, SCFSealTime=%d, SCFScrollItem=%d, SCFScrollTime=%d, SCFMarried=%d, SCFMarryHusbandWife='%s', SCFGensFamily=%d, SCFGensContribution=%d,SCFGensDateLeave=NULL,SCFGensRecvReward=%d WHERE Name = '%s'",
				sManager->m_DBName,lpMsg->MasterLevel,lpMsg->MasterPoints,sManager->m_DBResets,lpMsg->Resets,lpMsg->PCPoints,lpMsg->CShop_SealItem,lpMsg->CShop_SealTime,lpMsg->CShop_ScrollItem,lpMsg->CShop_ScrollTime,lpMsg->Married,szNameMarry,lpMsg->GensFamily,lpMsg->GensCont,lpMsg->GensRecvRewItem,szName);
		}else
		{
			if(lpMsg->GensNotAllow == 0)
			{
				sManager->db.ExecQuery("UPDATE %s.dbo.Character SET SCFMasterLevel=%d, SCFMasterPoints=%d, %s=%d, SCFPCPoints=%d, SCFSealItem=%d, SCFSealTime=%d, SCFScrollItem=%d, SCFScrollTime=%d, SCFMarried=%d, SCFMarryHusbandWife='%s', SCFGensFamily=%d, SCFGensContribution=%d,SCFGensRecvReward=%d WHERE Name = '%s'",
					sManager->m_DBName,lpMsg->MasterLevel,lpMsg->MasterPoints,sManager->m_DBResets,lpMsg->Resets,lpMsg->PCPoints,lpMsg->CShop_SealItem,lpMsg->CShop_SealTime,lpMsg->CShop_ScrollItem,lpMsg->CShop_ScrollTime,lpMsg->Married,szNameMarry,lpMsg->GensFamily,lpMsg->GensCont,lpMsg->GensRecvRewItem,szName);
			}else
			{
				sManager->db.ExecQuery("UPDATE %s.dbo.Character SET SCFMasterLevel=%d, SCFMasterPoints=%d, %s=%d, SCFPCPoints=%d, SCFSealItem=%d, SCFSealTime=%d, SCFScrollItem=%d, SCFScrollTime=%d, SCFMarried=%d, SCFMarryHusbandWife='%s', SCFGensFamily=%d, SCFGensContribution=%d,SCFGensDateLeave=getdate(),SCFGensRecvReward=%d WHERE Name = '%s'",
					sManager->m_DBName,lpMsg->MasterLevel,lpMsg->MasterPoints,sManager->m_DBResets,lpMsg->Resets,lpMsg->PCPoints,lpMsg->CShop_SealItem,lpMsg->CShop_SealTime,lpMsg->CShop_ScrollItem,lpMsg->CShop_ScrollTime,lpMsg->Married,szNameMarry,lpMsg->GensFamily,lpMsg->GensCont,lpMsg->GensRecvRewItem,szName);
			}
		}
		sManager->db.Fetch();
		sManager->db.Close();
	}
}

void GDGetPlayerExtraInfo(cDataServer * sManager, int aIndex, int pIndex, char * Name, char * AccountID)
{
	SDHP_GETPLAYEREXTRAINFO_SAVE pResult = {0};

	PHeadSetB((LPBYTE)&pResult, SCFExDB_GSRecv_GetPlayerExtraInfo, sizeof(pResult));
	//PHeadSetB((LPBYTE)&pResult, 0x10, sizeof(pResult));
	memcpy(pResult.AccountID, AccountID, 10);
	memcpy(pResult.Name, Name, 10);	
	pResult.aIndex = pIndex;
	
	if((SQLSyntexCheck(pResult.Name) == TRUE)	&&
	   (SpaceSyntexCheck(pResult.Name) == TRUE))
	{

		sManager->db.ExecQuery("SELECT SCFMasterLevel, SCFMasterPoints, %s, SCFPCPoints, SCFSealItem, SCFSealTime, SCFScrollItem, SCFScrollTime, SCFMarried, SCFMarryHusbandWife, SCFGensFamily, SCFGensContribution,SCFGensRank,SCFGensRecvReward,SCFGensDD = (CAST((DATEDIFF(dd,SCFGensDateLeave,GETDATE())) as int)+1) FROM %s.dbo.Character WHERE Name = '%s'",			sManager->m_DBResets,sManager->m_DBName,Name);
		if ( sManager->db.Fetch() != SQL_NO_DATA ) 
		{
			pResult.MasterLevel = sManager->db.GetAsInteger("SCFMasterLevel"); 
			pResult.MasterPoints = sManager->db.GetAsInteger("SCFMasterPoints"); 
			pResult.Resets = sManager->db.GetAsInteger(sManager->m_DBResets); 
			pResult.PCPoints = sManager->db.GetAsInteger("SCFPCPoints"); 

			pResult.CShop_SealItem = sManager->db.GetAsInteger("SCFSealItem");
			pResult.CShop_SealTime = sManager->db.GetAsInteger("SCFSealTime");
			pResult.CShop_ScrollItem = sManager->db.GetAsInteger("SCFScrollItem");
			pResult.CShop_ScrollTime = sManager->db.GetAsInteger("SCFScrollTime");

			pResult.Married = (BYTE) sManager->db.GetAsInteger("SCFMarried"); 
			sManager->db.GetAsString("SCFMarryHusbandWife", pResult.MarriedName);

			//----------------------------------------------------------------------

			pResult.Family = sManager->db.GetAsInteger("SCFGensFamily"); 
			pResult.Contribution = sManager->db.GetAsInteger("SCFGensContribution");
			pResult.Rank = sManager->db.GetAsInteger("SCFGensRank");
			pResult.RecvRewItem = sManager->db.GetAsInteger("SCFGensRecvReward");
			int Dif = sManager->db.GetAsInteger("SCFGensDD"); 
			if(Dif == 0 || Dif > 4)
			{
			}else
			{
				pResult.NotAllow = 1;
			}
			sManager->db.Close();

			//if(pResultGens.Family > 0)
			//{
			//	db.ExecQuery("SELECT Rank = ROW_NUMBER()OVER (ORDER BY SCFGensContribution DESC) FROM %s.dbo.Character WHERE Name = '%s' AND SCFGensFamily = %d",g_Config.m_sDbName,lpMsg->Name,pResultGens.Family);
			//	if ( db.Fetch() != SQL_NO_DATA ) 
			//	{
			//		pResultGens.Rank = db.GetAsInteger("Rank"); 
			//		db.Close();
			//	}
			//}

		}
		sManager->db.Close();

		sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
	}
}

void GJSetStatusBan(cDataServer * sManager, int aIndex, SDHP_SETSTATUSBAN_INFOSAVE * lpMsg)
{
	char szName[11]={0};
	memcpy(szName, lpMsg->Name, 10);

	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{
		if(lpMsg->Type == 0)
		{
			sManager->db.ExecQuery("UPDATE %s.dbo.Character SET CtlCode = %d WHERE Name = '%s'",sManager->m_DBName,lpMsg->Ban,szName);
		}else
		{
			sManager->db.ExecQuery("UPDATE %s.dbo.MEMB_INFO SET bloc_code = %d WHERE memb___id = '%s'",sManager->m_DBName2,lpMsg->Ban,szName);
		}
		
		sManager->db.Fetch();
		sManager->db.Close();
	}
}


void GJSetPlayerMarryForceDivorse(cDataServer * sManager, int aIndex, SDHP_SETMARRYDIVORSE_INFOSAVE * lpMsg)
{
	char szName[11]={0};
	memcpy(szName, lpMsg->Name, 10);
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{
		sManager->db.ExecQuery("UPDATE %s.dbo.Character SET SCFMarried=0, SCFMarryHusbandWife='' WHERE Name = '%s'",sManager->m_DBName,szName);
		sManager->db.Fetch();
		sManager->db.Close();
	}
}



void GJSetAccountExtraInfo(cDataServer * sManager, int aIndex, SDHP_GETACCOUNTEXTRAINFO_SAVE * lpMsg)
{
	char szAccount[11]={0};
	memcpy(szAccount, lpMsg->AccountID, 10);
	
	sManager->db.ExecQuery("UPDATE %s.dbo.MEMB_INFO SET SCFIsVip=%d, SCFVipMoney=%d, SCFVipDays=(CASE WHEN (%d-DateDiff(dd ,out__days,getdate())) > 0 THEN %d-DateDiff(dd ,out__days,getdate()) ELSE 0 END), SCFWareVipCount=%d, out__days=getdate(), SCFLuckyCoins=%d, SCFExtWarehouse=%d WHERE memb___id = '%s'",
		sManager->m_DBName2,lpMsg->Vip,lpMsg->VipMoney,
		lpMsg->VipDays,lpMsg->VipDays,
		lpMsg->WarehouseVipCount,lpMsg->LuckyCoins,lpMsg->ExWarehouse,szAccount);
	//sManager->db.ExecQuery("UPDATE %s.dbo.MEMB_INFO SET SCFIsVip=%d, SCFVipMoney=%d, SCFVipDays=%d-DateDiff(dd ,out__days,getdate()), SCFWareVipCount=%d, out__days=getdate(), SCFLuckyCoins=%d, SCFExtWarehouse=%d WHERE memb___id = '%s'",
	//	sManager->m_DBName2,lpMsg->Vip,lpMsg->VipMoney,lpMsg->VipDays,lpMsg->WarehouseVipCount,lpMsg->LuckyCoins,lpMsg->ExWarehouse,szAccount);
	sManager->db.Fetch();
	sManager->db.Close();

	#if (WL_PROTECT==1)
		int MyCheckVar;   
		VM_START_WITHLEVEL(30)
			CHECK_PROTECTION(MyCheckVar, 0x62585479)  	 
			if (MyCheckVar != 0x62585479)
			{				
				if(rand()%10 < 4)
					exit(1);
			}
		VM_END
	#endif
}

void GDSetWarehouseMoneyEx(cDataServer * sManager, int aIndex, SDHP_WAREHOUSEMONEYEX_SAVE * aRecv)
{
	//OutputDebugString("GDSetWarehouseMoney\n");

	char szAccountID[11]	= {0};
	// ----
	//OutputDebugString("memcpy(szAccountID, aRecv->AccountID, 10);\n");
	memcpy(szAccountID, aRecv->AccountID, 10);
	// ----

	if(aRecv->ExNum == 0)
		sManager->db.ExecQuery("UPDATE warehouse SET Money=%d WHERE AccountID='%s'", aRecv->Money, szAccountID);
	else
		sManager->db.ExecQuery("UPDATE %s.dbo.ExtendedWarehouse SET Money=%d WHERE AccountID='%s' AND SCFExtWare=%d",sManager->m_DBName, aRecv->Money, szAccountID,aRecv->ExNum);

	sManager->db.Fetch();
	sManager->db.Close();
}

void DGGetWarehouseListEx(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB_SCFEXDB * aRecv)
{
	char szAccountID[11]={0};
	memcpy(szAccountID, aRecv->AccountID, 10);

	SDHP_GETWAREHOUSEDB_SAVE_SCFEXDB pResult;
	SDHP_GETWAREHOUSEDB_S6E2_SAVE_SCFEXDB pResult2;

	memset(pResult.dbItems, -1, sizeof(pResult.dbItems));
	memset(pResult2.dbItems, -1, sizeof(pResult2.dbItems));

	if(aRecv->IsS6E2 == 0)
	{
		PHeadSetW((LPBYTE)&pResult, SCFExDB_GSRecv_GetWarehouseList_1, sizeof(pResult));
		memcpy(pResult.AccountID, szAccountID, 10);
		pResult.aIndex = aRecv->aIndex;
		pResult.ExNum = aRecv->ExNum;
	}else
	{
		PHeadSetW((LPBYTE)&pResult2, SCFExDB_GSRecv_GetWarehouseListS6E2, sizeof(pResult2));
		memcpy(pResult2.AccountID, szAccountID, 10);
		pResult2.aIndex = aRecv->aIndex;
		pResult2.ExNum = aRecv->ExNum;
	}
	
	BOOL bRet;
	if(aRecv->ExNum == 0)
		bRet= sManager->db.ExecQuery("SELECT AccountID FROM %s.dbo.warehouse WHERE AccountID='%s'",sManager->m_DBName, szAccountID);
	else
		bRet= sManager->db.ExecQuery("SELECT AccountID FROM %s.dbo.ExtendedWarehouse WHERE AccountID='%s' AND SCFExtWare=%d",sManager->m_DBName, szAccountID,aRecv->ExNum);
	
	if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
	{
		sManager->db.Close();
		if(aRecv->ExNum == 0)
			sManager->db.ExecQuery("INSERT INTO %s.dbo.warehouse (AccountID, Money, EndUseDate, DbVersion) VALUES ('%s',0, getdate(), 3)",sManager->m_DBName, szAccountID);
		else
			sManager->db.ExecQuery("INSERT INTO %s.dbo.ExtendedWarehouse (AccountID, Money, EndUseDate, DbVersion, SCFExtWare) VALUES ('%s',0, getdate(), 3, %d)",sManager->m_DBName, szAccountID,aRecv->ExNum);

		sManager->db.Fetch();
		sManager->db.Close();

		SDHP_GETWAREHOUSEDB_RESULT_SCFEXDB pMsg;

		PHeadSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_GetWarehouseNoItemEx_1, sizeof(pMsg));
		pMsg.aIndex = aRecv->aIndex;
		pMsg.ExNum = aRecv->ExNum;
		memcpy(pMsg.AccountID, szAccountID, 10);

		sManager->DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	sManager->db.Close();

	if(aRecv->ExNum == 0)
		sManager->db.ExecQuery("SELECT Money, DbVersion, pw FROM %s.dbo.warehouse WHERE AccountID='%s'",sManager->m_DBName, szAccountID);
	else
		sManager->db.ExecQuery("SELECT Money, DbVersion, pw FROM %s.dbo.ExtendedWarehouse WHERE AccountID='%s' AND SCFExtWare=%d",sManager->m_DBName, szAccountID,aRecv->ExNum);

	sManager->db.Fetch();
	if(aRecv->IsS6E2 == 0)
	{
		pResult.Money = sManager->db.GetAsInteger("Money");
		pResult.DbVersion = sManager->db.GetAsInteger("DbVersion");
		pResult.pw = sManager->db.GetAsInteger("pw");
	}else
	{
		pResult2.Money = sManager->db.GetAsInteger("Money");
		pResult2.DbVersion = sManager->db.GetAsInteger("DbVersion");
		pResult2.pw = sManager->db.GetAsInteger("pw");
	}
	sManager->db.Close();

	char szTemp[128];
	if(aRecv->ExNum == 0)
		wsprintf(szTemp, "SELECT Items FROM %s.dbo.warehouse WHERE AccountID='%s'",sManager->m_DBName, szAccountID);
	else
		wsprintf(szTemp, "SELECT Items FROM %s.dbo.ExtendedWarehouse WHERE AccountID='%s' AND SCFExtWare=%d",sManager->m_DBName, szAccountID,aRecv->ExNum);

	if(aRecv->IsS6E2 == 0)
	{
		if ( sManager->db.GetAsBinary(szTemp, pResult.dbItems) < 0 )
			memset(pResult.dbItems, -1, sizeof(pResult.dbItems));
	}else
	{
		if ( sManager->db.GetAsBinary(szTemp, pResult2.dbItems) < 0 )
			memset(pResult2.dbItems, -1, sizeof(pResult2.dbItems));
	}

	sManager->db.Close();

	if(aRecv->IsS6E2 == 0)
		sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
	else
		sManager->DataSend(aIndex, (LPBYTE)&pResult2, sizeof(pResult2));
}


void GDSetWarehouseListEx(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB_SAVE_SCFEXDB * aRecv)
{
	char szAccountID[11]={0};
	memcpy(szAccountID, aRecv->AccountID, 10);

	if(aRecv->ExNum == 0)
		sManager->db.ExecQuery("UPDATE %s.dbo.warehouse SET Money=%d, pw=%d, DbVersion=%d WHERE AccountID='%s'",
		sManager->m_DBName,aRecv->Money, aRecv->pw, aRecv->DbVersion, szAccountID);
	else
		sManager->db.ExecQuery("UPDATE %s.dbo.ExtendedWarehouse SET Money=%d, pw=%d, DbVersion=%d WHERE AccountID='%s' AND SCFExtWare=%d",
			sManager->m_DBName,aRecv->Money, aRecv->pw, aRecv->DbVersion, szAccountID,aRecv->ExNum);

	sManager->db.Fetch();
	sManager->db.Close();

	char szTemp[128];
	if(aRecv->ExNum == 0)
		wsprintf(szTemp, "UPDATE %s.dbo.warehouse SET Items=? WHERE AccountID='%s'", sManager->m_DBName,szAccountID);
	else
		wsprintf(szTemp, "UPDATE %s.dbo.ExtendedWarehouse SET Items=? WHERE AccountID='%s' AND SCFExtWare=%d",sManager->m_DBName, szAccountID,aRecv->ExNum);

	sManager->db.SetAsBinary(szTemp, aRecv->dbItems, sizeof(aRecv->dbItems));
	sManager->db.Fetch();
	sManager->db.Close();
}

void GDSetWarehouseList_S6E2(cDataServer * sManager, int aIndex, SDHP_GETWAREHOUSEDB_S6E2_SAVE_SCFEXDB * aRecv)
{
	char szAccountID[11]={0};
	memcpy(szAccountID, aRecv->AccountID, 10);

	if(aRecv->ExNum == 0)
		sManager->db.ExecQuery("UPDATE %s.dbo.warehouse SET Money=%d, pw=%d, DbVersion=%d WHERE AccountID='%s'",
			sManager->m_DBName,aRecv->Money, aRecv->pw, aRecv->DbVersion, szAccountID);
	else
		sManager->db.ExecQuery("UPDATE %s.dbo.ExtendedWarehouse SET Money=%d, pw=%d, DbVersion=%d WHERE AccountID='%s' AND SCFExtWare=%d",
			sManager->m_DBName,aRecv->Money, aRecv->pw, aRecv->DbVersion, szAccountID,aRecv->ExNum);

	sManager->db.Fetch();
	sManager->db.Close();

	char szTemp[128];
	if(aRecv->ExNum == 0)
		wsprintf(szTemp, "UPDATE %s.dbo.warehouse SET Items=? WHERE AccountID='%s'", sManager->m_DBName,szAccountID);
	else
		wsprintf(szTemp, "UPDATE %s.dbo.ExtendedWarehouse SET Items=? WHERE AccountID='%s' AND SCFExtWare=%d",sManager->m_DBName, szAccountID,aRecv->ExNum);

	sManager->db.SetAsBinary(szTemp, aRecv->dbItems, sizeof(aRecv->dbItems));
	sManager->db.Fetch();
	sManager->db.Close();
}

void DGGetPetBot(cDataServer * sManager, int aIndex, SDHP_GETACCOUNTEXTRAINFO * aRecv)
{
	char szAccountID[11]={0};
	memcpy(szAccountID, aRecv->AccountID, 10);

	SDHP_GETPETBOT pResult;
	memset(&pResult,0,sizeof(SDHP_GETPETBOT));

	pResult.aIndex = aRecv->aIndex;

	PHeadSetW((LPBYTE)&pResult, SCFExDB_GSRecv_GetPetBot, sizeof(pResult));

	pResult.aIndex = aRecv->aIndex;
	
	BOOL bRet = sManager->db.ExecQuery("SELECT AccountID,Name,cLevel,Class,Experience,Power,Defense,Life,MaxLife,Mana,MaxMana,MapNumber,MapPosX,MapPosY,BotLife,BotMaxLife,BotLvlUpDefense,BotLvlUpPower,BotLvlUpLife,BotLvlUpMana,BotLvlUpExp,BotLvlUpMaxLevel,BotVersion FROM %s.dbo.BotPet WHERE AccountID='%s' AND BotLife < BotMaxLife",sManager->m_DBName, szAccountID);
	
	if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
	{
		pResult.Result = 0;
	}else
	{
		pResult.Result = 1;
		sManager->db.GetAsString("AccountID", pResult.AccountID);
		sManager->db.GetAsString("Name", pResult.Name);	
		pResult.Level = (short)sManager->db.GetAsInteger("cLevel"); 
		pResult.Class = (BYTE)sManager->db.GetAsInteger("Class");
		pResult.Exp = sManager->db.GetAsInteger("Experience");
		pResult.Power = sManager->db.GetAsInteger("Power"); 
		pResult.Defense = sManager->db.GetAsInteger("Defense");
		pResult.Life = (WORD)sManager->db.GetAsFloat("Life");
		pResult.MaxLife = (WORD)sManager->db.GetAsFloat("MaxLife");
		pResult.Mana = (WORD)sManager->db.GetAsFloat("Mana");
		pResult.MaxMana = (WORD)sManager->db.GetAsFloat("MaxMana");
		pResult.MapNumber= (BYTE)sManager->db.GetAsInteger("MapNumber");
		pResult.MapX = (BYTE)sManager->db.GetAsInteger("MapPosX");
		pResult.MapY = (BYTE)sManager->db.GetAsInteger("MapPosY");

		pResult.BotLife = sManager->db.GetAsInteger("BotLife");
		pResult.BotMaxLife = sManager->db.GetAsInteger("BotMaxLife");
		pResult.BotLvlUpDefense = sManager->db.GetAsInteger("BotLvlUpDefense");
		pResult.BotLvlUpPower = sManager->db.GetAsInteger("BotLvlUpPower");
		pResult.BotLvlUpLife = sManager->db.GetAsInteger("BotLvlUpLife");
		pResult.BotLvlUpMana = sManager->db.GetAsInteger("BotLvlUpMana");
		pResult.BotLvlUpExp = sManager->db.GetAsInteger("BotLvlUpExp");
		pResult.BotLvlUpMaxLevel = sManager->db.GetAsInteger("BotLvlUpMaxLevel");
		pResult.BotVersion = sManager->db.GetAsInteger("BotVersion");

		char szTemp[1728];
		BYTE btTemp[1728]={0};

		sManager->db.Close();
		wsprintf(szTemp, "SELECT Inventory from %s.dbo.BotPet where Name='%s'",sManager->m_DBName, pResult.Name);
		if ( sManager->db.GetAsBinary(szTemp, btTemp) == -1 )
			memset(btTemp, -1, sizeof(btTemp));

		memcpy(pResult.dbInventory, btTemp, sizeof(pResult.dbInventory));
		memset(btTemp, 0, sizeof(btTemp));
		sManager->db.Close();

		wsprintf(szTemp, "SELECT MagicList from %s.dbo.BotPet where Name='%s'",sManager->m_DBName, pResult.Name);
		if ( sManager->db.GetAsBinary(szTemp, btTemp) == -1 ) 
			memset(btTemp, -1, sizeof(btTemp));
		memcpy(pResult.dbMagicList, btTemp, sizeof(pResult.dbMagicList));
		memset(btTemp, 0, sizeof(btTemp));
	}

	sManager->db.Close();
	sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
}

void DGSetPetBot(cDataServer * sManager, int aIndex, SDHP_GETPETBOT * aRecv)
{
	char szAccountID[11]={0};
	char Name[11]={0};
	memcpy(szAccountID, aRecv->AccountID, 10);
	memcpy(Name, aRecv->Name, 10);
	
	if((SQLSyntexCheck(Name) == TRUE)	&&
	   (SpaceSyntexCheck(Name) == TRUE))
	{
		BOOL bRet = sManager->db.ExecQuery("SELECT AccountID,Name FROM %s.dbo.BotPet WHERE AccountID='%s' AND Name='%s'",sManager->m_DBName, szAccountID,Name);
		
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			sManager->db.Close();
			//Insert
			sManager->db.ExecQuery("INSERT INTO %s.dbo.BotPet (AccountID, Name, cLevel, Class, Experience, Power, Defense, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY, BotLife, BotMaxLife, BotLvlUpDefense, BotLvlUpPower, BotLvlUpLife, BotLvlUpMana, BotLvlUpExp, BotLvlUpMaxLevel, BotVersion, CDate) VALUES ('%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, GETDATE())", sManager->m_DBName,aRecv->AccountID,aRecv->Name,aRecv->Level ,aRecv->Class,aRecv->Exp,aRecv->Power,aRecv->Defense,aRecv->Life,aRecv->MaxLife,aRecv->Mana,aRecv->MaxMana,aRecv->MapNumber,aRecv->MapX,aRecv->MapY,aRecv->BotLife,aRecv->BotMaxLife,aRecv->BotLvlUpDefense,aRecv->BotLvlUpPower,aRecv->BotLvlUpLife,aRecv->BotLvlUpMana,aRecv->BotLvlUpExp,aRecv->BotLvlUpMaxLevel,aRecv->BotVersion);
			sManager->db.Fetch();
			sManager->db.Close();
			
			char szTemp[1728];
			wsprintf(szTemp, "UPDATE %s.dbo.BotPet SET Inventory=? WHERE AccountID='%s' AND Name='%s'",sManager->m_DBName, szAccountID,Name);
			sManager->db.SetAsBinary(szTemp, aRecv->dbInventory, sizeof(aRecv->dbInventory));
			sManager->db.Fetch();
			sManager->db.Close();
			
			wsprintf(szTemp, "UPDATE %s.dbo.BotPet SET MagicList=? WHERE AccountID='%s' AND Name='%s'",sManager->m_DBName, szAccountID,Name);
			sManager->db.SetAsBinary(szTemp, aRecv->dbMagicList, sizeof(aRecv->dbMagicList));
			sManager->db.Fetch();
			sManager->db.Close();
		}else
		{
			sManager->db.Close();
			//Update
			sManager->db.ExecQuery("UPDATE %s.dbo.BotPet SET cLevel=%d, Class=%d, Experience=%d, Power=%d, Defense=%d, Life=%d, MaxLife=%d, Mana=%d, MaxMana=%d, MapNumber=%d, MapPosX=%d, MapPosY=%d, BotLife=%d, BotMaxLife=%d, BotLvlUpDefense=%d, BotLvlUpPower=%d, BotLvlUpLife=%d, BotLvlUpMana=%d, BotLvlUpExp=%d, BotLvlUpMaxLevel=%d, BotVersion=%d WHERE AccountID = '%s' AND Name = '%s'", sManager->m_DBName,aRecv->Level ,aRecv->Class,aRecv->Exp,aRecv->Power,aRecv->Defense,aRecv->Life,aRecv->MaxLife,aRecv->Mana,aRecv->MaxMana,aRecv->MapNumber,aRecv->MapX,aRecv->MapY,aRecv->BotLife,aRecv->BotMaxLife,aRecv->BotLvlUpDefense,aRecv->BotLvlUpPower,aRecv->BotLvlUpLife,aRecv->BotLvlUpMana,aRecv->BotLvlUpExp,aRecv->BotLvlUpMaxLevel,aRecv->BotVersion,aRecv->AccountID,aRecv->Name);
			sManager->db.Fetch();
			sManager->db.Close();
			
			char szTemp[1728];
			wsprintf(szTemp, "UPDATE %s.dbo.BotPet SET Inventory=? WHERE AccountID='%s' AND Name='%s'",sManager->m_DBName, szAccountID,Name);
			sManager->db.SetAsBinary(szTemp, aRecv->dbInventory, sizeof(aRecv->dbInventory));
			sManager->db.Fetch();
			sManager->db.Close();
			
			wsprintf(szTemp, "UPDATE %s.dbo.BotPet SET MagicList=? WHERE AccountID='%s' AND Name='%s'",sManager->m_DBName, szAccountID,Name);
			sManager->db.SetAsBinary(szTemp, aRecv->dbMagicList, sizeof(aRecv->dbMagicList));
			sManager->db.Fetch();
			sManager->db.Close();
		}
	}
}

void GDCreateResultPetBot(cDataServer * sManager, int aIndex, SDHP_PETBOTCREATERESULT * aRecv)
{
	char Name[11]={0};
	memcpy(Name, aRecv->Name, 10);	
	SDHP_PETBOTCREATERESULT pResult = {0};

	PHeadSetB((LPBYTE)&pResult, SCFExDB_GSRecv_CreateResultPetBot, sizeof(pResult));
	pResult.aIndex = aRecv->aIndex;
	pResult.result = 0;
	pResult.BotPetNum = aRecv->BotPetNum;
	memcpy(pResult.Name, aRecv->Name, 10);

	if((SQLSyntexCheck(pResult.Name) == TRUE)	&&
	   (SpaceSyntexCheck(pResult.Name) == TRUE))
	{
		BOOL bRet = sManager->db.ExecQuery("SELECT Name FROM %s.dbo.BotPet WHERE Name='%s'",sManager->m_DBName,Name);
		
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			sManager->db.Close();
			bRet = sManager->db.ExecQuery("SELECT Name FROM %s.dbo.Character WHERE Name='%s'",sManager->m_DBName,Name);
			if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
			{
				pResult.result = 1;
			}
		}
		sManager->db.Close();
		sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
	}
}

void GensGuildFamily(cDataServer * sManager, int aIndex, SDHP_GENSGUILD_INFO_REQUEST * lpMsg)
{
	SDHP_GENSGUILD_INFO_GET pResult = {0};
	PHeadSetB((LPBYTE)&pResult, SCFExDB_GSRecv_GetGuildGensInfo, sizeof(pResult));

	char szName[11]={0};
	char gMaster[11]={0};
	memcpy(szName, lpMsg->GuildName, sizeof(lpMsg->GuildName));
	memcpy(pResult.GuildName, lpMsg->GuildName,sizeof(lpMsg->GuildName));

	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{
		sManager->db.ExecQuery("SELECT SCFG_Gens,G_Master FROM %s.dbo.Guild WHERE G_Name = '%s'",sManager->m_DBName,lpMsg->GuildName);
		if ( sManager->db.Fetch() != SQL_NO_DATA ) 
		{
			pResult.Family = sManager->db.GetAsInteger("SCFG_Gens");  
			sManager->db.GetAsString("G_Master",gMaster);
			sManager->db.Close();

			BOOL bRet = sManager->db.ExecQuery("SELECT SCFGensFamily FROM %s.dbo.Character WHERE Name = '%s'",sManager->m_DBName,gMaster);		
			if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
			{
				//sManager->db.Close();
			}else
			{
				int OtherFam = sManager->db.GetAsInteger("SCFGensFamily");

				if(OtherFam != pResult.Family)
				{
					sManager->db.Close();
					pResult.Family = OtherFam;
					sManager->db.ExecQuery("UPDATE %s.dbo.Guild SET SCFG_Gens=%d WHERE G_Master = '%s'",sManager->m_DBName,pResult.Family,gMaster);
					sManager->db.Fetch();
				}
			}
			sManager->db.Close();
		}
		sManager->db.Close();
		sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
	}
}

void GensLeaveFamily(cDataServer * sManager, int aIndex,SDHP_SETPLAYERGENSLEAVEINFO_SAVE * lpMsg)
{
	char szName[11]={0};
	char szAccountID[11]={0};
	char szTemp[50];
	memcpy(szName, lpMsg->Name, sizeof(lpMsg->Name));
	memcpy(szAccountID, lpMsg->AccountID, sizeof(lpMsg->AccountID));
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		wsprintf(szTemp,"%d%d%d",sysTime.wYear,sysTime.wMonth,sysTime.wDay);
			
		sManager->db.ExecQuery("UPDATE %s.dbo.Character SET SCFGensFamily=0,SCFGensContribution=0,SCFGensRank=0,SCFGensDateLeave=GETDATE() WHERE AccountID = '%s' AND Name = '%s'", sManager->m_DBName,szTemp,szAccountID,szName);
		sManager->db.Fetch();
		sManager->db.Close();
	}
}


//SIEGE THINGS

void GS_DGAnsHuntZoneOpen(cDataServer * sManager, int aIndex, SDHP_GETHUNTZONEOPEN * lpMsg)
{
	SDHP_SETHUNTZONEOPEN pResult = {0};
	PHeadSubSetB((LPBYTE)&pResult, 0x40,0x00,sizeof(pResult));

	sManager->db.ExecQuery("SELECT HUNT_ZONE_OPEN FROM %s.dbo.MuExtraData where MAP_SRV=%d",
		sManager->m_DBName,lpMsg->mapServer);
	if ( sManager->db.Fetch() != SQL_NO_DATA ) 
	{
		pResult.state = sManager->db.GetAsInteger("HUNT_ZONE_OPEN"); 
	}
	sManager->db.Close();

	sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
}

void GS_DGGetHuntZoneOpen(cDataServer * sManager, int aIndex, SDHP_SETHUNTZONEOPEN * lpMsg)
{		
	sManager->db.ExecQuery("UPDATE %s.dbo.MuExtraData SET HUNT_ZONE_OPEN=%d where MAP_SRV=%d", 
		sManager->m_DBName,lpMsg->state,lpMsg->mapServer);
	sManager->db.Close();
}

//END SIEGE THIGNS

void GensRankingUpdate(cDataServer * sManager)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	if(sysTime.wDay > 7)
	{
		sManager->db.ExecQuery("EXEC %s.dbo.SCF_GENSRANKINGUPDATE %d",sManager->m_DBName,1);
		sManager->db.Fetch();
		sManager->db.Close();	
		sManager->db.ExecQuery("EXEC %s.dbo.SCF_GENSRANKINGUPDATE %d",sManager->m_DBName,2);
		sManager->db.Fetch();
		sManager->db.Close();
	}
}

void ClearCoins(cDataServer * sManager)
{
	tm* today;
	time_t ltime;
	time(&ltime);
	

	today=localtime(&ltime);
	if(today->tm_mday == 1)
	{
		if(today->tm_hour == 0 && today->tm_min == 0)
		{
			sManager->db.ExecQuery("INSERT %s.dbo.LuckyCoinsRank (Account,LuckyCoins,Date) SELECT TOP 5 memb___id, SCFLuckyCoins,getdate() FROM MEMB_INFO WHERE SCFLuckyCoins > 0 ORDER BY SCFLuckyCoins Desc",sManager->m_DBName2);
			sManager->db.Fetch();
			sManager->db.Close();

			sManager->db.ExecQuery("UPDATE %s.dbo.MEMB_INFO SET SCFLuckyCoins=0",sManager->m_DBName2);
			sManager->db.Fetch();
			sManager->db.Close();

			PMSG_DEFRESULT pResult = {0};			
			PHeadSetB((LPBYTE)&pResult, SCFExDB_GSRecv_GetCleanLuckyCoinsInfo, sizeof(pResult));
			pResult.result = 1;
			sManager->DataSendAll((LPBYTE)&pResult, sizeof(pResult));
		}
	}
}



void DGGetGuildWarehouseList(cDataServer * sManager, int aIndex, SDHP_GETGUILDWAREHOUSEDB * aRecv)
{
	char szAccountID[11]={0};
	memcpy(szAccountID, aRecv->AccountID, 10);
	char Guild[11]={0};
	memcpy(Guild, aRecv->G_Name, 10);


	SDHP_GETWAREHOUSEDB_SAVE_SCFEXDB pResult;
	SDHP_GETWAREHOUSEDB_S6E2_SAVE_SCFEXDB pResult2;

	memset(pResult.dbItems, -1, sizeof(pResult.dbItems));
	memset(pResult2.dbItems, -1, sizeof(pResult2.dbItems));

	if(aRecv->IsS6E2 == 0)
	{
		PHeadSetW((LPBYTE)&pResult, SCFExDB_GSRecv_GetWarehouseList_2, sizeof(pResult));
		memcpy(pResult.AccountID, szAccountID, 10);
		pResult.aIndex = aRecv->aIndex;
	}else
	{
		PHeadSetW((LPBYTE)&pResult2, SCFExDB_GSRecv_GetGuildWarehouseList, sizeof(pResult2));
		memcpy(pResult2.AccountID, szAccountID, 10);
		pResult2.aIndex = aRecv->aIndex;
	}
	
	BOOL bRet = sManager->db.ExecQuery("SELECT G_Name FROM %s.dbo.GuildWarehouse WHERE G_Name='%s'",sManager->m_DBName, Guild);
	
	if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
	{
		sManager->db.Close();
		sManager->db.ExecQuery("INSERT INTO %s.dbo.GuildWarehouse (G_Name, Money, EndUseDate, DbVersion) VALUES ('%s',0, getdate(), 3)",sManager->m_DBName, Guild);

		sManager->db.Fetch();
		sManager->db.Close();

		SDHP_GETWAREHOUSEDB_RESULT_SCFEXDB pMsg;

		PHeadSetB((LPBYTE)&pMsg, SCFExDB_GSRecv_GetWarehouseNoItemEx_2, sizeof(pMsg));
		pMsg.aIndex = aRecv->aIndex;
		memcpy(pMsg.AccountID, szAccountID, 10);

		sManager->DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	sManager->db.Close();

	sManager->db.ExecQuery("SELECT Money, DbVersion, pw FROM %s.dbo.GuildWarehouse WHERE G_Name='%s'",sManager->m_DBName, Guild);

	sManager->db.Fetch();
	if(aRecv->IsS6E2 == 0)
	{
		pResult.Money = sManager->db.GetAsInteger("Money");
		pResult.DbVersion = sManager->db.GetAsInteger("DbVersion");
		pResult.pw = sManager->db.GetAsInteger("pw");
	}else
	{
		pResult2.Money = sManager->db.GetAsInteger("Money");
		pResult2.DbVersion = sManager->db.GetAsInteger("DbVersion");
		pResult2.pw = sManager->db.GetAsInteger("pw");
	}
	sManager->db.Close();

	char szTemp[128];
	wsprintf(szTemp, "SELECT Items FROM %s.dbo.GuildWarehouse WHERE G_Name='%s'",sManager->m_DBName, Guild);
	
	if(aRecv->IsS6E2 == 0)
	{
		if ( sManager->db.GetAsBinary(szTemp, pResult.dbItems) < 0 )
			memset(pResult.dbItems, -1, sizeof(pResult.dbItems));
	}else
	{
		if ( sManager->db.GetAsBinary(szTemp, pResult2.dbItems) < 0 )
			memset(pResult2.dbItems, -1, sizeof(pResult2.dbItems));
	}

	sManager->db.Close();

	if(aRecv->IsS6E2 == 0)
	{
		sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
	}else
		sManager->DataSend(aIndex, (LPBYTE)&pResult2, sizeof(pResult2));
}


void GDSetGuildWarehouseList(cDataServer * sManager, int aIndex, SDHP_GETGUILDWAREHOUSEDB_SAVE * aRecv)
{
	char szAccountID[11]={0};
	memcpy(szAccountID, aRecv->AccountID, 10);
	char Guild[11]={0};
	memcpy(Guild, aRecv->G_Name, 10);

	sManager->db.ExecQuery("UPDATE %s.dbo.GuildWarehouse SET Money=%d, pw=%d, DbVersion=%d WHERE G_Name='%s'",
		sManager->m_DBName,aRecv->Money, aRecv->pw, aRecv->DbVersion, Guild);

	sManager->db.Fetch();
	sManager->db.Close();

	char szTemp[128];
	wsprintf(szTemp, "UPDATE %s.dbo.GuildWarehouse SET Items=? WHERE G_Name='%s'", sManager->m_DBName,Guild);

	sManager->db.SetAsBinary(szTemp, aRecv->dbItems, sizeof(aRecv->dbItems));
	sManager->db.Fetch();
	sManager->db.Close();
}


void GDSetGuildWarehouseList_S6E2(cDataServer * sManager, int aIndex, SDHP_GETGUILDWAREHOUSEDB_S6E2_SAVE * aRecv)
{
	char szAccountID[11]={0};
	memcpy(szAccountID, aRecv->AccountID, 10);
	char Guild[11]={0};
	memcpy(Guild, aRecv->G_Name, 10);

	sManager->db.ExecQuery("UPDATE %s.dbo.GuildWarehouse SET Money=%d, pw=%d, DbVersion=%d WHERE G_Name='%s'",
		sManager->m_DBName,aRecv->Money, aRecv->pw, aRecv->DbVersion, Guild);

	sManager->db.Fetch();
	sManager->db.Close();

	char szTemp[8000];
	wsprintf(szTemp, "UPDATE %s.dbo.GuildWarehouse SET Items=? WHERE G_Name='%s'", sManager->m_DBName,Guild);

	sManager->db.SetAsBinary(szTemp, aRecv->dbItems, sizeof(aRecv->dbItems));
	sManager->db.Fetch();
	sManager->db.Close();
}


void GDDelGuildWarehouseList(cDataServer * sManager, int aIndex, SDHP_GETGUILDWAREHOUSEDB * aRecv)
{
	char szAccountID[11]={0};
	memcpy(szAccountID, aRecv->AccountID, 10);
	char Guild[11]={0};
	memcpy(Guild, aRecv->G_Name, 10);

	sManager->db.ExecQuery("DELETE FROM GuildWarehouse where G_Name='%s'", Guild);

	sManager->db.Fetch();
	sManager->db.Close();
}

void GDGetPlayerMasterSkillInfo2(cDataServer * sManager, int aIndex, SDHP_GETPLAYEREXTRAINFO * aRecv)
{
	//GDGetPlayerMasterSkillInfo(sManager,aIndex,aRecv->aIndex,aRecv->Name,aRecv->AccountID);
	SDHP_GETPLAYERMASTERSKILL_SAVE result = {0};	
	PHeadSetW((LPBYTE)&result, SCFExDB_GSRecv_SetPlayerMasterSkill, sizeof(result));

	memcpy(result.AccountID, aRecv->AccountID, 10);
	memcpy(result.Name, aRecv->Name, 10);
	result.aIndex = aRecv->aIndex;

	char Name[11]={0};
	memcpy(Name, aRecv->Name, 10);

	char AccountID[11]={0};
	memcpy(AccountID, aRecv->AccountID, 10);
	
	if((SQLSyntexCheck(Name) == TRUE)	&&
	   (SpaceSyntexCheck(Name) == TRUE))
	{
		char szTemp2[10000];
		BYTE btTemp2[(sizeof(MASTERSKILLS)*MAX_SKILLTREE)]={0};

		BOOL bRet = sManager->db.ExecQuery("SELECT Name FROM %s.dbo.Character WHERE Name = '%s' and AccountID = '%s'",sManager->m_DBName, Name, AccountID);
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			memset(btTemp2, 0, sizeof(btTemp2));
		} else 
		{
			sManager->db.Close();
			wsprintf(szTemp2, "SELECT SCFMasterSkill FROM %s.dbo.Character where Name = '%s' and AccountID = '%s'",sManager->m_DBName, Name, AccountID);
			if ( sManager->db.GetAsBinary(szTemp2, btTemp2) == -1 )
				memset(btTemp2, 0, sizeof(btTemp2));
		}
		memcpy(result.Skill, btTemp2, sizeof(result.Skill));
		sManager->db.Close();
		sManager->DataSend(aIndex, (LPBYTE)&result, sizeof(result));
	}
}

//void GDGetPlayerMasterSkillInfo(cDataServer * sManager, int aIndex, int pIndex, char * Name, char * AccountID)
//{
//	SDHP_GETPLAYERMASTERSKILL_SAVE pResult = {0};
//	PHeadSetW((LPBYTE)&pResult, SCFExDB_GSRecv_SetPlayerMasterSkill, sizeof(pResult));
//	memcpy(pResult.AccountID, AccountID, 10);
//	memcpy(pResult.Name, Name, 10);	
//	pResult.aIndex = pIndex;
//	
//	
//	if((SQLSyntexCheck(pResult.Name) == TRUE)	&&
//	   (SpaceSyntexCheck(pResult.Name) == TRUE))
//	{
//		char szTemp2[10000];
//		BYTE btTemp2[(sizeof(MASTERSKILLS)*MAX_SKILLTREE)]={0};
//
//		BOOL bRet = sManager->db.ExecQuery("SELECT Name FROM %s.dbo.Character WHERE Name = '%s' and AccountID = '%s'",sManager->m_DBName, Name, AccountID);
//		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
//		{
//			memset(btTemp2, 0, sizeof(btTemp2));
//		} else 
//		{
//			sManager->db.Close();
//			wsprintf(szTemp2, "SELECT SCFMasterSkill FROM %s.dbo.Character where Name = '%s' and AccountID = '%s'",sManager->m_DBName, Name, AccountID);
//			if ( sManager->db.GetAsBinary(szTemp2, btTemp2) == -1 )
//				memset(btTemp2, 0, sizeof(btTemp2));
//		}
//		memcpy(pResult.Skill, btTemp2, sizeof(pResult.Skill));
//		sManager->db.Close();
//		sManager->DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult));
//	}
//}

void GJSetPlayerMasterSkillInfo(cDataServer * sManager, int aIndex, SDHP_GETPLAYERMASTERSKILL_SAVE * lpMsg)
{
	char szName[11]={0};
	memcpy(szName, lpMsg->Name, 10);
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
	   (SpaceSyntexCheck(szName) == TRUE))
	{
		char szTemp[10000];
		wsprintf(szTemp, "UPDATE %s.dbo.Character SET SCFMasterSkill=? WHERE Name='%s'",sManager->m_DBName, szName);
		sManager->db.SetAsBinary(szTemp, (LPBYTE)lpMsg->Skill, sizeof(lpMsg->Skill));
		sManager->db.Fetch();
		sManager->db.Close();
	}
}

void GDGetAccountExtraInfo(cDataServer * sManager, int aIndex, int pIndex, char * AccountID)
{
	SDHP_GETACCOUNTEXTRAINFO_SAVE pResult = {0};
	bool newCalc=false;

	PHeadSetB((LPBYTE)&pResult, SCFExDB_GSRecv_GetAccountExtraInfo, sizeof(pResult));
	memcpy(pResult.AccountID, AccountID, 10);
	pResult.aIndex = pIndex;

	sManager->db.ExecQuery("SELECT SCFIsVip, SCFVipMoney, SCFVipDays, SCFWareVipCount, DateDiff(dd , out__days,getdate()) as dateDifference, SCFLuckyCoins, SCFExtWarehouse FROM %s.dbo.MEMB_INFO WHERE memb___id = '%s'",sManager->m_DBName2,AccountID); //Open1
	//db.ExecQuery("SELECT SCFIsVip, SCFVipMoney, SCFVipDays, SCFWareVipCount FROM %s.dbo.MEMB_INFO WHERE memb___id = '%s'",g_Config.m_sDbName2,lpMsg->AccountID);
	if ( sManager->db.Fetch() != SQL_NO_DATA ) 
	{
		int dateDif = sManager->db.GetAsInteger("dateDifference");
		int VipDays = sManager->db.GetAsInteger("SCFVipDays");
		int VipMoney = sManager->db.GetAsInteger("SCFVipMoney");
		int IsVip = sManager->db.GetAsInteger("SCFIsVip");
		int VipWareCount = sManager->db.GetAsInteger("SCFWareVipCount");
		int LuckyCoins = sManager->db.GetAsInteger("SCFLuckyCoins");
		int ExWarehouse =sManager-> db.GetAsInteger("SCFExtWarehouse");

		pResult.Vip = 0;
		pResult.VipDays = 0;

		if (dateDif < VipDays)
		{
			pResult.Vip = IsVip; 
			pResult.VipDays = VipDays-dateDif;

			if (dateDif > 0)
			{
				sManager->db.Close(); //Close 1	
				sManager->db.ExecQuery("UPDATE %s.dbo.MEMB_INFO SET out__days=getdate(), SCFVipDays=%d WHERE memb___id = '%s'",
					sManager->m_DBName2,pResult.VipDays,AccountID); //Open2
				sManager->db.Fetch();
			}
		}else 
		{
			if((g_Config.VipAutoRenew == 1) && (IsVip == 1))
			{
				if(VipMoney >= g_Config.VipValue)
				{
					pResult.Vip = 1; 
					pResult.VipDays = g_Config.VipDays;
					VipMoney = VipMoney - g_Config.VipValue;

					sManager->db.Close();//Close 1
					sManager->db.ExecQuery("UPDATE %s.dbo.MEMB_INFO SET out__days=getdate(), SCFVipMoney=%d, SCFVipDays=%d WHERE memb___id = '%s'",
						sManager->m_DBName2,VipMoney,g_Config.VipDays,AccountID); //Open3
					sManager->db.Fetch();
				}
			}
		}

		pResult.VipMoney = VipMoney;
		pResult.WarehouseVipCount = VipWareCount;
		pResult.LuckyCoins = LuckyCoins;
		pResult.ExWarehouse = ExWarehouse;
		
		sManager->db.Close(); //Close 1 or 2 or 3

		BOOL bRet = sManager->db.ExecQuery("SELECT AccountID,BotLife,BotMaxLife FROM %s.dbo.BotPet WHERE AccountID='%s' AND BotLife < BotMaxLife",sManager->m_DBName, AccountID); //Open4
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			pResult.HaveBot = 0;
		}else
		{
			pResult.HaveBot = 1;
		}

		sManager->db.Close(); //Close 4

		bRet = bRet = sManager->db.ExecQuery("SELECT COUNT(*) as cnt FROM %s.dbo.TitanLotteryPlayers WHERE AccountID='%s'", sManager->m_DBName, AccountID); //Open 5
		if ( sManager->db.Fetch() == SQL_NO_DATA || bRet == FALSE )
		{
			pResult.LotteryCnt = 0;
		}else
		{
			pResult.LotteryCnt = sManager->db.GetAsInteger("cnt");
		}

	}
	sManager->db.Close(); //Close 1 or 5
	
	sManager->DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
}


void GetAccountExtraInfo(cDataServer * sManager, int aIndex, SDHP_GETACCOUNTEXTRAINFO * aRecv)
{
	char szAccountID[11]	= {0};
	memcpy(szAccountID, aRecv->AccountID, 10);
	//SCFExDB Adds
	GDGetAccountExtraInfo(sManager,aIndex,aRecv->aIndex,szAccountID);
}

void SetResetLog(cDataServer * sManager, SDHP_SETRESET_LOG * aRecv)
{
	char szAccountID[11]			= {0};
	char szName[11]					= {0};

	memcpy(szAccountID, aRecv->AccountID, 10);
	memcpy(szName, aRecv->Name, 10);
	
	if((SQLSyntexCheck(szName) == TRUE)	&&
		(SpaceSyntexCheck(szName) == TRUE))
	{
		sManager->db.ExecQuery("INSERT INTO %s.dbo.Resets_Log (AccountID,Name,Reset,r_Type,Date) VALUES ('%s','%s',%d,%d,GETDATE())", sManager->m_DBName,szAccountID,szName,aRecv->Reset,aRecv->Type);
		sManager->db.Fetch();
	}
	sManager->db.Close();
}