#include "stdafx.h"
#include "config.h"
#include "winutil.h"
#include "JSProtocol.h"
#include "Network/WzUdp.h"
#include "../stdafx.h"
#include "Log.h"
#include "MD5.h"

WzUdp g_ConnecServerUDP;
int g_MuLoginUserDataCount = 0;
JOIN_SERVER_USER_DATA g_MuLoginUserData[MAX_LOGIN_USER];

struct PMSG_NOTIFY_USERS
{
	PBMSG_HEAD h;
	BYTE Unk1;
	WORD ServerCode;
	BYTE Unk2;
	WORD iCurUserCount;	// 8
	BYTE Unk3[3];
	WORD iMaxUserCount;	//	C
};

struct PMSG_NOTIFY_USERS2
{
	PBMSG_HEAD h;
	int Port;
	WORD iCurUserCount;	// 8
	WORD iMaxUserCount;	//	C
};

struct PMSG_NOTIFY_GSSTATUS
{
	PBMSG_HEAD h;
	WORD ServerCode;
	BYTE State;
};

struct PMSG_NOTIFY_GSSTATUS2
{
	PBMSG_HEAD h;
	int Port;
	BYTE State;
};

#pragma pack(1)

//std::vector<std::string> g_vecMuUseLog;
std::vector<tagUSER_DATA> g_vecMapMove;
int g_MuLoginServerDataCount = 0;

#pragma pack()

void JSProtocolCore(cJoinServer * sManager,int aIndex, BYTE Head, unsigned char *pRecv, int aLen) 
{
	switch ( Head )
	{
		case 0x00:
			JoinServerLogin(sManager, aIndex, (SDHP_SERVERINFO *)pRecv);
			break;
		case 0x01:
			JGPAccountRequest(sManager, aIndex, (SDHP_IDPASS *)pRecv);
			break;
		case 0x02:
			GJPAccountFail(sManager, aIndex, (SDHP_JOINFAIL *)pRecv);
			break;
		case 0x04:
			GJPAccountBlock(sManager, aIndex, (SDHP_COMMAND_BLOCK *)pRecv);
			break;
		case 0x05:
			GJPUserClose(sManager, aIndex, (SDHP_USERCLOSE_ID *)pRecv);
			break;
		case 0x06:
			GCJoinBillCheckSend(aIndex, (SDHP_BILLSEARCH *) pRecv);
			break;
		case 0x30:
			LoveHeartEventRecv(sManager, aIndex, (SDHP_LOVEHEARTEVENT *) pRecv);
			break;
		case 0x31:
			LoveHeartCreateSend(sManager, aIndex, (SDHP_LOVEHEARTCREATE *) pRecv);
			break;
		case 0x7A:
			GJReqMapSvrMove(sManager, aIndex, (PMSG_REQ_MAPSVRMOVE *) pRecv);
			break;
		case 0x7B:
			GJReqMapSvrAuth(sManager, aIndex, (PMSG_REQ_MAPSVRAUTH *) pRecv);
			break;
		case 0x7C:
			GJNotifyMaxUserCount(sManager, aIndex, (PMSG_NOTIFY_MAXUSER *) pRecv);
			break;
		case 0xA1:
			NoticeRecv(sManager, aIndex, (SDHP_NOTICE *) pRecv);
			break;
		case 0xA2:
			MngPro_UserNoticeSend(sManager, aIndex,(PMSG_USERNOTICE *)pRecv);
			break;
		case 0xA3:
			MngPro_GameBlock(sManager, aIndex,(PMSG_GAME_BLOCK *)pRecv);
			break;
		case 0x99:
			GJAlreadyConnect(sManager, aIndex, (SDHP_USERCLOSE_ID *)pRecv);
			break;
		case 0x98:
			GetGoldChannelInfo(sManager, aIndex, (SDHP_SETGOLDCHANNELTIME *)pRecv);
			break;
		 //default:
			//sManager->vLogAdd(4,"Unknow MU PROTCOOL TYPE %d 0x%d",pRecv[2]);
			//gObjDel(aIndex);
			//return;
	}

}

// --------- protocol


int MuLoginFindServer(cJoinServer * sManager, WORD ServerCode)
{
	for(int i=0;i<MAX_LOGIN_SERVER;i++)
	{
		if ( sManager->eObj[i].m_wServerCode == ServerCode )
		{
			return i;
		}

	}
	return -1;
}

BOOL MuLoginAddServer(cJoinServer * sManager, LPTSTR ServerName, WORD ServerCode, WORD Port)
{
	if ( MuLoginFindServer(sManager, ServerCode) != -1 )
	{
		LogAdd("GameServer %s trying to connect with a ServerCode %d already used",
			ServerName, ServerCode);

		return FALSE;
	}

	for(int i=0;i<MAX_LOGIN_SERVER;i++)
	{
		if ( sManager->eObj[i].m_szServerName[0] == 0 )
		{
			if ( sManager->eObj[i].m_szServerName[1] == 0 )
			{
				if ( sManager->eObj[i].m_wServerCode == -1 )
				{
					sManager->eObj[i].m_wPort = Port;
					sManager->eObj[i].m_wServerCode = ServerCode;

					strcpy(sManager->eObj[i].m_szServerName, ServerName);
					g_MuLoginServerDataCount++;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


BOOL MuLoginAddUser(WORD ServerCode, char * szAccountID,int aIndex,DWORD GoldTime)
{
	for(int i=0;i<MAX_LOGIN_USER;i++)
	{
		if ( g_MuLoginUserData[i].m_AccoundID[0] == 0 )
		{
			if ( g_MuLoginUserData[i].m_AccoundID[1] == 0 )
			{
				strcpy(g_MuLoginUserData[i].m_AccoundID, szAccountID);
				g_MuLoginUserData[i].iUserNumber = aIndex;
				g_MuLoginUserData[i].m_ServerCode = ServerCode;
				g_MuLoginUserData[i].m_bRequestMapSvrMove = false;
				g_MuLoginUserData[i].GoldTime = GoldTime;
				
				g_MuLoginUserDataCount++;
				//UpdateInstanceWindowTitle();
				LogAdd("[Object] User %s has join",g_MuLoginUserData[i].m_AccoundID);
				return TRUE;
			}
		}
	}

	return FALSE;
}

int MuLoginFindUser(char * szAccountID)
{
	for(int i=0;i<MAX_LOGIN_USER;i++)
	{
		if(JSCaseSensitive == 1)
		{
			if ( g_MuLoginUserData[i].m_AccoundID[0] == szAccountID[0] )
			{
				if ( g_MuLoginUserData[i].m_AccoundID[1] == szAccountID[1] )
				{
					if ( lstrcmp(g_MuLoginUserData[i].m_AccoundID, szAccountID) == 0 )
					{
						return i;
					}
				}
			}
		}else
		{
			if(!stricmp(g_MuLoginUserData[i].m_AccoundID,szAccountID))
				return i;
		}
	}

	return -1;
}

void MuLoginDeleteUser(int Index)
{
	LogAdd("[Player] %s has been deleted", g_MuLoginUserData[Index].m_AccoundID);
	memset(g_MuLoginUserData[Index].m_AccoundID, 0, 10);
	g_MuLoginUserData[Index].m_ServerCode = -1;
	g_MuLoginUserData[Index].m_bRequestMapSvrMove = false;
	g_MuLoginUserData[Index].iUserNumber = -1;
	g_MuLoginUserData[Index].GoldTime = 0;
	memset(&g_MuLoginUserData[Index].pMapMove , 0 , sizeof(g_MuLoginUserData[Index].pMapMove));					
	g_MuLoginUserDataCount--;
	//UpdateInstanceWindowTitle();	
}

void MuLoginDeleteUser(char * szAccountID)
{
	for(int i=0;i<MAX_LOGIN_USER;i++)
	{
		if(JSCaseSensitive == 1)
		{
			if ( g_MuLoginUserData[i].m_AccoundID[0] == szAccountID[0] )
			{
				if ( g_MuLoginUserData[i].m_AccoundID[1] == szAccountID[1] )
				{
					if ( lstrcmp(g_MuLoginUserData[i].m_AccoundID, szAccountID) == 0 )
					{	
						MuLoginDeleteUser(i);
						break;
					}
				}
			}
		}else
		{
			if(!stricmp(g_MuLoginUserData[i].m_AccoundID,szAccountID))
			{
				MuLoginDeleteUser(i);
				break;
			}
		}
	}
}

bool AllowGoldTime(DWORD GTime)
{
	time_t t = time(NULL);
	localtime(&t);
	if(GTime <= (DWORD)t)
	{
		return false;
	}
	return true;
}

void KickUserFunc(cJoinServer * sManager, char * AccountID)
{
	int i = MuLoginFindUser(AccountID);

	if(i != -1)
	{
		int aIndex = MuLoginFindServer(sManager, g_MuLoginUserData[i].m_ServerCode);
		if(aIndex != -1)
		{
			SDHP_BILLKILLUSER pMsg={0};
	
			PHeadSetB((LPBYTE)&pMsg.h, 0x07, sizeof(pMsg));
			strcpy(pMsg.Id, g_MuLoginUserData[i].m_AccoundID);
			pMsg.Number = g_MuLoginUserData[i].iUserNumber;
			sManager->DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		}
		
		UserClose(sManager,AccountID);
	}
}

DWORD __stdcall CheckGoldTime(void *pArg)
{
	cJoinServer *	sManager = (cJoinServer *)pArg;

	while(true)
	{
		if(sManager->UseGoldChannel == 1)
		{
			for(int i=0;i<MAX_LOGIN_USER;i++)
			{
				if ( g_MuLoginUserData[i].iUserNumber != -1 )
				{
					int aIndex = MuLoginFindServer(sManager,g_MuLoginUserData[i].m_ServerCode);
					if(aIndex != -1)
					{
						if (sManager->eObj[aIndex].m_GoldChannel == true)
						{
							if(AllowGoldTime(g_MuLoginUserData[i].GoldTime) == false)
							{
								SDHP_BILLKILLUSER pMsg={0};

								PHeadSetB((LPBYTE)&pMsg.h, 0x7, sizeof(pMsg));
								strcpy(pMsg.Id, g_MuLoginUserData[i].m_AccoundID);
								pMsg.Number = g_MuLoginUserData[i].iUserNumber;
								sManager->DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
							}
						}
					}
				}
			}
		}
		Sleep(1000);
	}
}

HANDLE hGoldChannelThread;
void StartCheckGoldTime(cJoinServer * sManager)
{
	DWORD threadID;
	hGoldChannelThread = CreateThread(NULL, 0, &CheckGoldTime, sManager, 0, &threadID );
}

void ShowStatics(cJoinServer * sManager)
{
	for(int i=0;i<MAX_LOGIN_SERVER;i++)
	{
		if(sManager->isConnected(i) == true)
		{
			sManager->vLogAddC(3,"[Statics][%d] {%s:%d} Online: [%d/%d]",i,sManager->eObj[i].m_szServerName,sManager->eObj[i].m_wServerCode,sManager->eObj[i].m_wCurrentUser,sManager->eObj[i].m_wMaxUser);
		}
	}
}

void GetGoldChannelInfo(cJoinServer * sManager, int aIndex, SDHP_SETGOLDCHANNELTIME * aRecv)
{
	int user = MuLoginFindUser(aRecv->AccountID);
	if(user != -1)
	{
		g_MuLoginUserData[user].GoldTime = aRecv->GoldChannelTime;
		SendGoldChannelTime(sManager, aRecv->AccountID,aRecv->GoldChannelTime);
	}
}

void UserRemove(cJoinServer * sManager, char * AccountID)
{
	if(sManager->SpecialMove == 0)
	{
		char szAccountID[11]={0};
		strcpy(szAccountID, AccountID);

		int iUserIndex = MuLoginFindUser(szAccountID);

		if ( g_MuLoginUserData[iUserIndex].m_bRequestMapSvrMove == true )
		{
			tagUSER_DATA pDataUser;
			for(std::vector<tagUSER_DATA>::iterator _Iter = g_vecMapMove.begin(); _Iter != g_vecMapMove.end() ; _Iter++)
			{
				pDataUser = (*(_Iter));
				
				if ( pDataUser.pMapServerMoveData.szAccountID[0] == szAccountID[0] )
				{
					if ( pDataUser.pMapServerMoveData.szAccountID[1] == szAccountID[1] )
					{
						if ( lstrcmp(pDataUser.pMapServerMoveData.szAccountID, szAccountID) == 0 )
						{
							g_vecMapMove.erase(_Iter);
							break;						
						}
					}
				}
			}
		}
	}
}


void UserClose(cJoinServer * sManager, char * AccountID)
{
	char	szAccountID[11]		= {0};
	if(strlen(AccountID) >= 10)
		strncpy(szAccountID,AccountID,10);
	else
		strcpy(szAccountID, AccountID);

	//sManager->db.ExecQuery("UPDATE MEMB_STAT SET ConnectStat = 0, DisConnectTM = getdate() WHERE memb___id = '%s'",szAccountID);
	sManager->db.ExecQuery("EXEC WZ_DISCONNECT_MEMB '%s'", szAccountID);
	sManager->db.Fetch();
	sManager->db.Close();	

	MuLoginDeleteUser(szAccountID);
}



void GJAlreadyConnect(cJoinServer * sManager, int aIndex, SDHP_USERCLOSE_ID * aRecv )
{
	char	szAccountID[11]		= {0};

	if(strlen(aRecv->szId) >= 10)
		strncpy(szAccountID,aRecv->szId,sizeof(aRecv->szId));
	else
		strcpy(szAccountID, aRecv->szId);
	
	int i = MuLoginFindUser(szAccountID);

	if(i != -1)
	{
		int cIndex = MuLoginFindServer(sManager, g_MuLoginUserData[i].m_ServerCode);
		if((cIndex != -1 && aIndex == cIndex) || cIndex == -1)
		{
			UserRemove(sManager,szAccountID);
			UserClose(sManager,szAccountID);
		}
	}
}

BOOL GJPCheckUserConnection(cJoinServer * sManager, int UserNum)
{		
	if(g_MuLoginUserData[UserNum].m_bRequestMapSvrMove == true)
	{
		UserRemove(sManager,g_MuLoginUserData[UserNum].m_AccoundID);
		UserClose(sManager, g_MuLoginUserData[UserNum].m_AccoundID);
		return FALSE;
	}

	if(sManager->UseSpecialDisconnect == 1)
	{
		SDHP_USERCONNECTION	pMsg={0};	

		PHeadSetB((LPBYTE)&pMsg, 0x99, sizeof(pMsg));
		strcpy(pMsg.Id, g_MuLoginUserData[UserNum].m_AccoundID);
	
		sManager->DataSend(g_MuLoginUserData[UserNum].m_ServerCode, (LPBYTE)&pMsg, pMsg.h.size);
		//sManager->DataSendAll((LPBYTE)&pMsg, pMsg.h.size);
	}

	return TRUE;
}

bool IsGoldChannel(int ServerIndex)
{
	int Val1 = ServerIndex / 20;
	int Val2 = ServerIndex - (Val1 * 20);
	if(Val2 <= 3)
		return true;
	return false;
}

void CSSendServerInfo(int SvrCode, BYTE State)
{
	PMSG_NOTIFY_GSSTATUS pMsg={0};
	PHeadSetB((LPBYTE)&pMsg, 0x03, sizeof(pMsg));
	pMsg.ServerCode = SvrCode;
	pMsg.State = State;
	g_ConnecServerUDP.SendData((LPBYTE)&pMsg, pMsg.h.size);
}

void CSSendServerInfo2(int Port, BYTE State)
{
	PMSG_NOTIFY_GSSTATUS2 pMsg={0};
	PHeadSetB((LPBYTE)&pMsg, 0x03, sizeof(pMsg));
	pMsg.Port = Port;
	pMsg.State = State;
	g_ConnecServerUDP.SendData((LPBYTE)&pMsg, pMsg.h.size);
}

void JoinServerLogin(cJoinServer * sManager, int aIndex, SDHP_SERVERINFO * lpMsg)
{
	SDHP_RESULT pResult = {0};

	PHeadSetB((LPBYTE)&pResult, 0x00, sizeof(pResult));
	pResult.Result = 1;

	if ( MuLoginAddServer(sManager,lpMsg->ServerName, lpMsg->ServerCode, lpMsg->Port) == TRUE )
	{
		sManager->vLogAdd("GameServer connected %s PORT : %d CODE : %d",
			lpMsg->ServerName, lpMsg->Port, lpMsg->ServerCode);

		sManager->eObj[aIndex].m_wServerCode = lpMsg->ServerCode;

		if(sManager->UseGoldChannel == 1)
			if(IsGoldChannel(lpMsg->ServerCode) == true)
				sManager->eObj[aIndex].m_GoldChannel = true;
		
		if(g_Config.UseTTCS == 0)
			CSSendServerInfo(lpMsg->ServerCode,1);
		else
			CSSendServerInfo2(lpMsg->Port,1);
		
		#if(JS_USE_VIP == 1)
			if(sDatabase->MakeSyntax("SELECT IsVip From T_ServerTypes WHERE ServerCode = '%d';",lpMsg->ServerCode))
			{
				SQLRETURN Result = sDatabase->Fetch();

				if (Result != SQL_NO_DATA )
				{
					Obj[aIndex].m_iVipCode = sDatabase->GetAsInteger("IsVip");
				}
			}
		#endif
	}
	else
	{
		pResult.Result = 0;
	}
	sManager->db.Close();
	sManager->DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
}


void JGPAccountRequest(cJoinServer * sManager, int aIndex, SDHP_IDPASS * lpMsg)
{
#if(JS_USE_VIP == 1)
	int		VipCode				= 0;
#endif
	int		VipDays				= 0;
	DWORD	GoldChannelTime		= 0;
	bool	GoldChannelZero		= true;

	char	szAccountID[11]		= {0};
	char	szIp[16]			= {0};
	char	szPass[11]			= {0};
	char	szTemp[100]			= {0};
	char	JoominNumber[19]	= {0};
	DWORD	dwAccKey			= {0};
	// ----
	int		Number				= lpMsg->Number;
	int		DBNumber			= 0;
	int		UserNumber			= 0;
	int		BlocCode			= 0;
	BYTE	Result				= 1;
	// ----
	MD5 pMD5Hash;
	// ----
	BuxConvert(lpMsg->Id, 10);
	BuxConvert(lpMsg->Pass, 10);
	// ----
	memcpy(szAccountID, lpMsg->Id , 10);
	memcpy(szPass, lpMsg->Pass , 10);
	memcpy(szIp, lpMsg->IpAddress, 15);
	// ----
	if((SQLSyntexCheck(szAccountID) == TRUE)	&&
	   (SQLSyntexCheck(szPass) == TRUE)			&&
	   (SpaceSyntexCheck(szAccountID) == TRUE))
	{	
		int con = MuLoginFindUser(szAccountID);
		if(con != -1)
		{
			if(GJPCheckUserConnection(sManager,con) == TRUE)
			{
				sManager->vLogAdd(" Account already connected ID : %s", szAccountID);
				JGOtherJoin(sManager, aIndex, szAccountID);
			}
			Result = 0x03;
		}else
		{
			//1
			if(sManager->db.ExecQuery("SELECT * FROM MEMB_INFO WHERE memb___id='%s'", szAccountID) == TRUE)
			{
				if(sManager->db.Fetch() != SQL_NO_DATA)
				{																							// ####
					DBNumber = sManager->db.GetAsInteger("memb_guid");
					BlocCode = sManager->db.GetAsInteger("bloc_code");
					// ----
					sManager->db.GetAsString("sno__numb", JoominNumber);
					// ----

					BlocCode = sManager->db.GetAsInteger("bloc_code");
#if(JS_USE_VIP == 1)
					VipCode = sManager->db.GetAsInteger("VipCode");
#endif
					VipDays = sManager->db.GetAsInteger("SCFVipDays");
					GoldChannelTime = sManager->db.GetAsInteger("GoldChannelTime");

					if(sManager->g_UseMD5 == 1)
					{
						//C 1
						sManager->db.Close();

						BYTE btBinaryPass[16] = {0};
						// ----
						// # MD5 Login
						// ----
						wsprintf(szTemp, "SELECT memb__pwd from MEMB_INFO where memb___id='%s'", szAccountID);
						//2
						if(sManager->db.GetAsBinary(szTemp, btBinaryPass) > 0)
						{
							dwAccKey = MakeAccountKey(szAccountID);
							// ----
							if(pMD5Hash.MD5_CheckValue(szPass, (char*)btBinaryPass, dwAccKey) == true)
							{
								BOOL bErrorFlag = FALSE;
								if (sManager->UseGoldChannel == 1)
								{
									if(sManager->eObj[aIndex].m_GoldChannel == true)
									{
										if(GoldChannelTime == 0)
										{
											Result = 10;
											bErrorFlag = TRUE;
										}else if(AllowGoldTime(GoldChannelTime) == false)
										{
											Result = 10;
											bErrorFlag = TRUE;
											GoldChannelZero = false;
										}
									}
								}
#if(JS_USE_VIP == 1)
								if( Obj[aIndex].m_iVipCode > 0)
								{
									if(VipDays <= 0)
									{
										Result = 10;
										bErrorFlag = TRUE;
										sManager->vLogAdd(" User Isnt VIP - ID : %s", szAccountID);
									}
									else if(VipCode < Obj[aIndex].m_iVipCode)
									{
										Result = 10;
										bErrorFlag = TRUE;
										sManager->vLogAdd(" User vip access is too low - ID : %s", szAccountID);
									}
								}
#endif
								if(bErrorFlag == FALSE)
									Result = 1;
							}else
							{
								Result = 0;
								// ----
								sManager->vLogAdd(" Wrong Password - ID : %s", szAccountID);
							}
						}
						else
						{
							Result = 0;
							// ----
							sManager->vLogAdd("Password field is blank - ID : %s", szAccountID);
						}

						//C 2
						sManager->db.Close();
					}
					else
					{
						// ----
						// # NORMAL Login
						// ----
						char szPassword[11] = {0};
						// ----
						sManager->db.GetAsString("memb__pwd", szPassword);
						// ----
						if(strcmp(szPass, szPassword) == 0)
						{
							BOOL bErrorFlag = FALSE;
							if (sManager->UseGoldChannel == 1)
							{
								if(sManager->eObj[aIndex].m_GoldChannel == true)
								{
									if(GoldChannelTime == 0)
									{
										Result = 10;
										bErrorFlag = TRUE;
									}else if(AllowGoldTime(GoldChannelTime) == false)
									{
										Result = 10;
										bErrorFlag = TRUE;
										GoldChannelZero = false;
									}
								}
							}

#if(JS_USE_VIP == 1)
							if( Obj[aIndex].m_iVipCode > 0)
							{
								if(VipDays <= 0)
								{
									Result = 10;
									bErrorFlag = TRUE;
									sManager->vLogAdd(" User Isnt VIP - ID : %s", szAccountID);
								}
								else if(VipCode < Obj[aIndex].m_iVipCode)
								{
									Result = 10;
									bErrorFlag = TRUE;
									sManager->vLogAdd(" User vip access is too low - ID : %s", szAccountID);
								}
							}
#endif

							if(bErrorFlag == FALSE)
								Result = 1;
						}
						else
						{
							Result = 0;

							sManager->vLogAdd(" Wrong Password - ID : %s", szAccountID);
						}

						//C 1
						sManager->db.Close();
					}

					// ----
					if(Result == 1)
					{
						if(BlocCode > 0)
						{
							Result = 0x05;
						}
						else
						{
							if ( MuLoginAddUser(sManager->eObj[aIndex].m_wServerCode, szAccountID, Number, GoldChannelTime) == FALSE )	
							{
								Result = 0x04;
							}
							else
							{	
								//sManager->db.Close();		//[Fix]DaRkav Extra Close
								int iServerIndex = MuLoginFindServer(sManager,sManager->eObj[aIndex].m_wServerCode);

								//3
								if ( iServerIndex != -1 )
									sManager->db.ExecQuery("EXEC WZ_CONNECT_MEMB '%s','%s','%s'",szAccountID, sManager->eObj[iServerIndex].m_szServerName, szIp);
								else
									sManager->db.ExecQuery("EXEC WZ_CONNECT_MEMB '%s','%s','%s'", szAccountID, "", szIp);

								sManager->db.Fetch();
								//C 3
								sManager->db.Close();
							}
						}
					}
				}																							// ####
				else
				{
					Result = 0x02;
					// ----
					sManager->vLogAdd(" Account doesn't exist - ID : %s", szAccountID);
				}
			}																								// ###
			else
			{
				//C 1
				sManager->db.Close();

				//sManager->vLogAdd("[GProtocol][JGPAccountRequest] :: Fail ExecFormat ID : %s",
				//	szAccountID);
				// ----
				Result = (UCHAR)0x0E;
			}
		}
	}																										// #
	else
	{
		//sManager->vLogAdd("[ANTIHACK] Special characters in... ID : %s",
		//		szAccountID);
		Result = 0x0E;
	}

	SDHP_IDPASSRESULT msg = {0};
	PHeadSetB((LPBYTE)&msg, 0x01, sizeof(msg));
	msg.result = Result;
	msg.UserNumber = UserNumber;
	msg.DBNumber = DBNumber;
	msg.Number = Number;

	if(strlen(JoominNumber) <= sizeof(msg.JoominNumber))
		strcpy(msg.JoominNumber,JoominNumber);
	else
		strncpy(msg.JoominNumber,JoominNumber,sizeof(msg.JoominNumber));

	strcpy(msg.Id,szAccountID);
	sManager->DataSend(aIndex, (LPBYTE)&msg, msg.h.size);
}

void SendGoldChannelTime(cJoinServer * sManager, char * szAccountID, DWORD GTime)
{
	sManager->db.ExecQuery("UPDATE MEMB_INFO SET GoldChannelTime = %d WHERE memb___id = '%s'", GTime,szAccountID);
	sManager->db.Fetch();
	sManager->db.Close();
}

void GJPAccountFail(cJoinServer * sManager, int aIndex, SDHP_JOINFAIL * aRecv)
{
	char	szAccountID[11]		= {0};
	if(strlen(aRecv->Id) >= 10)
		strncpy(szAccountID,aRecv->Id,10);
	else
		strcpy(szAccountID, aRecv->Id);

	//sManager->db.ExecQuery("UPDATE MEMB_STAT SET ConnectStat = 0, DisConnectTM = getdate() WHERE memb___id = '%s'",szAccountID);
	
	sManager->db.ExecQuery("EXEC WZ_DISCONNECT_MEMB '%s'", szAccountID);
	sManager->db.Fetch();
	sManager->db.Close();	

	MuLoginDeleteUser(szAccountID);
}

void GJPAccountBlock(cJoinServer * sManager, int aIndex, SDHP_COMMAND_BLOCK * aRecv)
{
	MuLoginDeleteUser(aRecv->Id);
	sManager->db.ExecQuery("UPDATE MEMB_INFO SET bloc_code = 1 WHERE memb___id = '%s'", aRecv->Id);

	sManager->db.Fetch();
	sManager->db.Close();
}

void GJPUserClose(cJoinServer * sManager, int aIndex, SDHP_USERCLOSE_ID * aRecv )
{	
	char	szAccountID[11]		= {0};

	if(strlen(aRecv->szId) >= 10)
		strncpy(szAccountID,aRecv->szId,sizeof(aRecv->szId));
	else
		strcpy(szAccountID, aRecv->szId);

	int i = MuLoginFindUser(szAccountID);
	
	if(i != -1)
		UserClose(sManager, szAccountID);
}

void GCJoinBillCheckSend(int aIndex, SDHP_BILLSEARCH * aRecv)
{
//	SDHP_BILLSEARCH_RESULT pResult={0};
//
////#error Still Need the Remian Time Check
//	PHeadSetB((LPBYTE)&pResult, 0x06, sizeof(pResult));
//	char szAccountID[11]={0};
//	memcpy(szAccountID, aRecv->Id, 10);
//	//CHECK
//	//strcpy(szAccountID, aRecv->Id);
}

void JGOtherJoin(cJoinServer * sManager, int aIndex, LPTSTR szAccountID)
{
	SDHP_OTHERJOINMSG pMsg = {0};

	PHeadSetB((LPBYTE)&pMsg.h, 0x08, sizeof(pMsg));

	if(strlen(szAccountID) >= 10)
		strncpy(pMsg.AccountID,szAccountID,sizeof(pMsg.AccountID));
	else
		strcpy(pMsg.AccountID, szAccountID);

	sManager->DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}

void JGKillUserSend(cJoinServer * sManager, int aIndex, LPTSTR szAccountID)
{
	SDHP_BILLKILLUSER pMsg={0};

	PHeadSetB((LPBYTE)&pMsg.h, 0x07, sizeof(pMsg));
	strcpy(pMsg.Id, szAccountID);

	pMsg.Number = MuLoginFindUser(szAccountID);

	sManager->DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}

void JGMailSend(cJoinServer * sManager, int aIndex, LPTSTR szAccountID,LPTSTR szName , LPTSTR Subject , LPTSTR Memo)
{
	PMSG_JG_MEMO_SEND pMsg={0};
	PHeadSetB((LPBYTE)&pMsg.h, 0x63, sizeof(pMsg));

	if(strlen(szAccountID) < sizeof(pMsg.TargetName))
		strcpy(pMsg.TargetName, szAccountID);
	else
		strncpy(pMsg.TargetName, szAccountID,sizeof(pMsg.TargetName));

	if(strlen(szName) < sizeof(pMsg.Name))
		strcpy(pMsg.Name, szName);
	else
		strncpy(pMsg.Name, szName,sizeof(pMsg.Name));

	if(strlen(Subject) < sizeof(pMsg.Subject))
		strcpy(pMsg.Subject, Subject);
	else
		strncpy(pMsg.Subject, Subject,sizeof(pMsg.Subject));

	if(strlen(Memo) < sizeof(pMsg.Memo))
		strcpy(pMsg.Memo, Memo);
	else
		strncpy(pMsg.Memo, Memo,sizeof(pMsg.Memo));
	
	pMsg.MemoSize = strlen(Memo);

	sManager->DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg) - pMsg.MemoSize);
}

void GJNoticeSend(cJoinServer * sManager, int aIndex, LPTSTR Notice)
{
	SDHP_NOTICE_S pMsg={0};
	PHeadSetB((LPBYTE)&pMsg.h, 0x40, sizeof(pMsg));

	if(strlen(Notice) < strlen(pMsg.Notice))
		strcpy(pMsg.Notice,Notice);
	else
		strncpy(pMsg.Notice,Notice,sizeof(pMsg.Notice));

	sManager->DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}

void LoveHeartEventRecv(cJoinServer * sManager, int aIndex, SDHP_LOVEHEARTEVENT * aRecv)
{
	DWORD dwHeartCount=0;
	SDHP_LOVEHEARTEVENT_RESULT pResult={0};
	PHeadSetB((LPBYTE)&pResult, 0x30, sizeof(pResult));
	pResult.Result = 0;
	strcpy(pResult.Name, aRecv->Name);

	//1
	if ( sManager->db.ExecQuery("SELECT heart_count FROM LoveHeartCount WHERE Number=0") == TRUE )
	{
		if ( sManager->db.Fetch() == SQL_NO_DATA )
		{
			//C 1
			sManager->db.Close();
			//2
			sManager->db.ExecQuery("UPDATE LoveHeartCount SET heart_count=0 WHERE Number=0");
			sManager->db.Fetch();
			//sManager->db.Close(); //[Fix]DaRkav Removed double Close
		}
		else
		{
			dwHeartCount = sManager->db.GetAsInteger("heart_count");
		}
	}
	//C 1,2
	sManager->db.Close();

	dwHeartCount++;

	if ( dwHeartCount > 217000 )
	{
		sManager->db.ExecQuery("UPDATE LoveHeartCount SET heart_count=0 WHERE Number=0");
		sManager->db.Fetch();	//[Fix]DaRKav added just in case
		sManager->db.Close();
	}

	sManager->db.ExecQuery("UPDATE LoveHeartCount SET heart_count= %d where Number=0", dwHeartCount);
	sManager->db.Fetch();
	sManager->db.Close();

	sManager->db.ExecQuery("INSERT INTO LoveHeartAll ( Number, Id, Server, Name ) Values (%d, '%s','%s','%s')",
		dwHeartCount, aRecv->Account, sManager->eObj[MuLoginFindServer(sManager,sManager->eObj[aIndex].m_wServerCode)].m_szServerName, aRecv->Name);
	sManager->db.Fetch();
	sManager->db.Close();

	int iIndex = MuLoginFindUser(aRecv->Account);

	if ( iIndex >= 0 )	// if Is Connected
	{
		if ( (rand()%217000) == iIndex )	// if Wins
		{
			int iServerIndex = MuLoginFindServer(sManager,sManager->eObj[aIndex].m_wServerCode);

			if ( iServerIndex != -1 )
				sManager->db.ExecQuery("INSERT INTO LoveHeart ( Number, Id, Server, Name ) Values (%d, '%s','%s','%s')", dwHeartCount, aRecv->Account, sManager->eObj[iServerIndex].m_szServerName, aRecv->Name);
			else
				sManager->db.ExecQuery("INSERT INTO LoveHeart ( Number, Id, Server, Name ) Values (%d, '%s','%s','%s')", dwHeartCount, aRecv->Account, "", aRecv->Name);

			sManager->db.Fetch();
			sManager->db.Close();
			pResult.Result = 1;

			if ( iServerIndex != iIndex )
				sManager->vLogAdd("[LOVE HEART] Event Winner : %s:%s - Server : %s",aRecv->Account, aRecv->Name, sManager->eObj[iServerIndex].m_szServerName);
			else
				sManager->vLogAdd("[LOVE HEART] Event Winner : %s:%s - Server : %s",aRecv->Account, aRecv->Name, "");
		}
	}	
	pResult.Number = dwHeartCount;
	sManager->DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);
}

void LoveHeartCreateSend(cJoinServer * sManager, int aIndex, SDHP_LOVEHEARTCREATE * aRecv)
{
	sManager->db.ExecQuery("UPDATE LoveHeartCount SET heart_count=heart_count+1  where Number=1");
	sManager->db.Fetch();
	sManager->db.Close();
}

void GJReqMapSvrMove(cJoinServer * sManager, int aIndex, PMSG_REQ_MAPSVRMOVE * aRecv)
{
	sManager->vLogAddTD("[MapSvrReqMove][%s][%s] %d->%d",aRecv->szAccountID,aRecv->szCharName,aRecv->wCurMapSvrCode,aRecv->wDstMapSvrCode);
	DWORD fResult = 0;

	if ( lstrlen(aRecv->szAccountID) < 1 )
	{
		fResult = 1;		
	}

	int iOldServerIndex = MuLoginFindServer(sManager,aRecv->wCurMapSvrCode);
	int iNewServerIndex = MuLoginFindServer(sManager,aRecv->wDstMapSvrCode);

	// No Dest Server Connected
	if ( iOldServerIndex == -1 || iNewServerIndex == -1 )
	{
		fResult = 2;
		sManager->vLogAddTD("GJReqMapSvrMove() -> ServerIndex = -1 ( cur:%d | dest: %d) Account:(%s)",aRecv->wCurMapSvrCode,aRecv->wDstMapSvrCode,aRecv->szAccountID);
	}
	// Limit User reached
	else if ( sManager->eObj[iNewServerIndex].m_wCurrentUser >= sManager->eObj[iNewServerIndex].m_wMaxUser )
	{
		fResult = 3;
		sManager->vLogAddTD("GJReqMapSvrMove -> Server(%d) UserLimit reached (cur:%d | max:%d)",aRecv->wDstMapSvrCode,sManager->eObj[iNewServerIndex].m_wCurrentUser,sManager->eObj[iNewServerIndex].m_wMaxUser);
	}

	PMSG_ANS_MAPSVRMOVE pResult = {0};

	PHeadSetB((LPBYTE)&pResult, 0x7A, sizeof(pResult));
	int iUserIndex=-1;

	// Some Other Checks
	if ( fResult == 0 )
	{
		iUserIndex = MuLoginFindUser(aRecv->szAccountID);

		// User not Connected
		if ( iUserIndex == -1 )
		{
			fResult = 1;sManager->vLogAddTD("GJReqMapSvrMove -> iUserIndex == -1 (%s)(%s)",aRecv->szAccountID,aRecv->szCharName);
		}
		// User Requested the Move
		else if ( g_MuLoginUserData[iUserIndex].m_bRequestMapSvrMove != false )
		{
			fResult = 4;
			sManager->vLogAddTD("GJReqMapSvrMove -> m_bRequestMapSvrMove == true (%s)(%s)",aRecv->szAccountID,aRecv->szCharName);
		}
		else
		{
			g_MuLoginUserData[iUserIndex].m_bRequestMapSvrMove = true;
			sManager->vLogAddTD("GJReqMapSvrMove -> fResult == SUCCESS, (%s)(%s) dest:(%d) map(%d)",
				aRecv->szAccountID,aRecv->szCharName,aRecv->wDstMapSvrCode,aRecv->wMapNumber);
		}
		
	}

	pResult.iIndex = aRecv->iIndex;
	pResult.btX = aRecv->btX;
	pResult.btY = aRecv->btY;
	strcpy(pResult.szAccountID, aRecv->szAccountID);
	strcpy(pResult.szCharName, aRecv->szCharName);

	pResult.wCurMapSvrCode = aRecv->wCurMapSvrCode;
	pResult.wDstMapSvrCode = aRecv->wDstMapSvrCode;
	pResult.iResult = fResult;
	pResult.iJoinAuthCode1 = GetTickCount();
	pResult.iJoinAuthCode2 = GetTickCount() + rand() % 10000;
	pResult.iJoinAuthCode3 = GetTickCount() + rand() % 777;
	pResult.iJoinAuthCode4 = GetTickCount() + rand() % 8911;
	pResult.wMapNumber = aRecv->wMapNumber;
	
	if(sManager->SpecialMove == 1)
	{
		g_MuLoginUserData[iUserIndex].pMapMove.btX = pResult.btX;
		g_MuLoginUserData[iUserIndex].pMapMove.btY = pResult.btY;
			
		if(strlen(pResult.szCharName) >= 10)
			strncpy(g_MuLoginUserData[iUserIndex].pMapMove.szCharName,pResult.szCharName,10);
		else
			strcpy(g_MuLoginUserData[iUserIndex].pMapMove.szCharName, pResult.szCharName);

		//strcpy(g_MuLoginUserData[iUserIndex].pMapMove.szCharName, pResult.szCharName);
		g_MuLoginUserData[iUserIndex].pMapMove.iJoinAuthCode1 = pResult.iJoinAuthCode1;
		g_MuLoginUserData[iUserIndex].pMapMove.iJoinAuthCode2 = pResult.iJoinAuthCode2;
		g_MuLoginUserData[iUserIndex].pMapMove.iJoinAuthCode3 = pResult.iJoinAuthCode3;
		g_MuLoginUserData[iUserIndex].pMapMove.iJoinAuthCode4 = pResult.iJoinAuthCode4;
		g_MuLoginUserData[iUserIndex].pMapMove.wMapNumber = pResult.wMapNumber;
		g_MuLoginUserData[iUserIndex].pMapMove.wCurMapSvrCode = pResult.wCurMapSvrCode;
		g_MuLoginUserData[iUserIndex].pMapMove.wDstMapSvrCode = pResult.wDstMapSvrCode;
	}else
	{
		tagUSER_DATA pUserData;

		if ( iUserIndex != -1 )
			pUserData.iUserNumber = g_MuLoginUserData[iUserIndex].iUserNumber;

		memcpy(&pUserData.pMapServerMoveData, &pResult, sizeof(pResult));
		g_vecMapMove.push_back(pUserData);
	}
	
	sManager->DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

}




void GJReqMapSvrAuth(cJoinServer * sManager, int aIndex, PMSG_REQ_MAPSVRAUTH * aRecv)
{
	DWORD fResult = 0;

	if ( lstrlen(aRecv->szAccountID) < 1 )
		fResult = 1;

	PMSG_ANS_MAPSVRAUTH pResult = {0};

	PHeadSetB((LPBYTE)&pResult, 0x7B, sizeof(pResult));
	strcpy(pResult.szAccountID, aRecv->szAccountID);
	strcpy(pResult.szCharName, aRecv->szCharName);
	pResult.iIndex = aRecv->iIndex;
	
	if ( fResult == 0 )
	{
		int iUserIndex = MuLoginFindUser(aRecv->szAccountID);

		// User Not Connected
		if ( iUserIndex == -1 )
			fResult = 1;

		if ( g_MuLoginUserData[iUserIndex].m_bRequestMapSvrMove == true )
		{			
			if(sManager->SpecialMove == 1)
			{
				bool Found = false;
				if(!strcmp(aRecv->szAccountID,g_MuLoginUserData[iUserIndex].m_AccoundID))
				{
					if(!strcmp(aRecv->szCharName,g_MuLoginUserData[iUserIndex].pMapMove.szCharName))
						Found = true;
				}
				if(Found == true)
				{
					if ( CHECK_LIMIT(g_MuLoginUserData[iUserIndex].pMapMove.wCurMapSvrCode, MAX_LOGIN_SERVER) &&
						 CHECK_LIMIT(g_MuLoginUserData[iUserIndex].pMapMove.wDstMapSvrCode, MAX_LOGIN_SERVER))
					{
						if ( aRecv->iJoinAuthCode1 == g_MuLoginUserData[iUserIndex].pMapMove.iJoinAuthCode1 &&
							 aRecv->iJoinAuthCode2 == g_MuLoginUserData[iUserIndex].pMapMove.iJoinAuthCode2 &&
							 aRecv->iJoinAuthCode3 == g_MuLoginUserData[iUserIndex].pMapMove.iJoinAuthCode3 &&
							 aRecv->iJoinAuthCode4 == g_MuLoginUserData[iUserIndex].pMapMove.iJoinAuthCode4 )
						{
							g_MuLoginUserData[iUserIndex].m_bRequestMapSvrMove = false;
							pResult.btX = g_MuLoginUserData[iUserIndex].pMapMove.btX;
							pResult.btY = g_MuLoginUserData[iUserIndex].pMapMove.btY;
							pResult.wPrevMapSvrCode = g_MuLoginUserData[iUserIndex].pMapMove.wCurMapSvrCode;
							pResult.wMapNumber = g_MuLoginUserData[iUserIndex].pMapMove.wMapNumber;

							sManager->db.ExecQuery("SELECT * FROM MEMB_INFO WHERE memb___id='%s'", aRecv->szAccountID);

							if ( sManager->db.Fetch() != SQL_NO_DATA )
							{
								pResult.iUserNumber = g_MuLoginUserData[iUserIndex].iUserNumber;
								sManager->db.GetAsString("sno__numb", pResult.cJoominNumber,sizeof(pResult.cJoominNumber));
								pResult.iDBNumber = sManager->db.GetAsInteger("memb_guid");
								pResult.btBlockCode = (BYTE)sManager->db.GetAsInteger("bloc_code");
							}
							sManager->db.Close();
			
							memset(&g_MuLoginUserData[iUserIndex].pMapMove , 0 , sizeof(g_MuLoginUserData[iUserIndex].pMapMove));
						}
						else
						{
							fResult = 2;
						}
					}
					else
					{
						fResult = 3;
					}
				}else
				{
					fResult = 4;
				}
			}else
			{
				tagUSER_DATA pDataUser;
				BOOL bFlagFound = FALSE;

				for(std::vector<tagUSER_DATA>::iterator _Iter = g_vecMapMove.begin(); _Iter != g_vecMapMove.end() ; _Iter++)
				{
					pDataUser = (*(_Iter));
					
					if ( pDataUser.pMapServerMoveData.szAccountID[0] == aRecv->szAccountID[0] )
					{
						if ( pDataUser.pMapServerMoveData.szAccountID[1] == aRecv->szAccountID[1] )
						{
							if ( strncmp(pDataUser.pMapServerMoveData.szAccountID, aRecv->szAccountID,10) == 0 )
							{
								if ( strncmp(pDataUser.pMapServerMoveData.szCharName, aRecv->szCharName,10) == 0 )
								{
									bFlagFound = TRUE;
									g_vecMapMove.erase(_Iter);
									break;
								}
							}
						}
					}
				}

				if ( bFlagFound == TRUE )
				{
					if ( CHECK_LIMIT(pDataUser.pMapServerMoveData.wCurMapSvrCode, MAX_LOGIN_SERVER) &&
						 CHECK_LIMIT(pDataUser.pMapServerMoveData.wDstMapSvrCode, MAX_LOGIN_SERVER)   )
					{
						if ( aRecv->iJoinAuthCode1 == pDataUser.pMapServerMoveData.iJoinAuthCode1 &&
							 aRecv->iJoinAuthCode2 == pDataUser.pMapServerMoveData.iJoinAuthCode2 &&
							 aRecv->iJoinAuthCode3 == pDataUser.pMapServerMoveData.iJoinAuthCode3 &&
							 aRecv->iJoinAuthCode4 == pDataUser.pMapServerMoveData.iJoinAuthCode4 )
						{
							g_MuLoginUserData[iUserIndex].m_bRequestMapSvrMove = false;
							pResult.btX = pDataUser.pMapServerMoveData.btX;
							pResult.btY = pDataUser.pMapServerMoveData.btY;
							pResult.wPrevMapSvrCode = pDataUser.pMapServerMoveData.wCurMapSvrCode;
							pResult.wMapNumber = pDataUser.pMapServerMoveData.wMapNumber;

							sManager->db.ExecQuery("SELECT * FROM MEMB_INFO WHERE memb___id='%s'", aRecv->szAccountID);

							if ( sManager->db.Fetch() != SQL_NO_DATA )
							{
								pResult.iUserNumber = pDataUser.iUserNumber;
								sManager->db.GetAsString("sno__numb", pResult.cJoominNumber,sizeof(pResult.cJoominNumber));
								pResult.iDBNumber = sManager->db.GetAsInteger("memb_guid");
								pResult.btBlockCode = (BYTE)sManager->db.GetAsInteger("bloc_code");
							}
							sManager->db.Close();
						}
						else
						{
							fResult = 2;
						}
					}
					else
					{
						fResult = 3;
					}
				}
				else
				{
					fResult = 4;
				}
			}
		}
		else
		{
			fResult = 4;
		}
	}

	pResult.iResult = fResult;

	sManager->DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size);

	sManager->vLogAddTD("GJReqMapSvrAuth() -> User(%s) Character(%s) fResult(%d)",
		pResult.szAccountID,pResult.szAccountID,pResult.iResult);
	sManager->vLogAddTD("[%s][%s] %d",aRecv->szAccountID,aRecv->szCharName,aRecv->wDstMapSvrCode);
}


void GJNotifyMaxUserCount(cJoinServer * sManager,int aIndex, PMSG_NOTIFY_MAXUSER * aRecv)
{
	int iIndex = MuLoginFindServer(sManager,aRecv->iSvrCode);

	if ( iIndex != -1 )
	{
		sManager->eObj[iIndex].m_wCurrentUser = aRecv->iCurUserCount;
		sManager->eObj[iIndex].m_wMaxUser = aRecv->iMaxUserCount;

		if(g_Config.UseTTCS == 0)
		{
			PMSG_NOTIFY_USERS pMsg={0};
			PHeadSetB((LPBYTE)&pMsg, 0x01, sizeof(pMsg));
			pMsg.Unk1=0xCC;
			pMsg.ServerCode = aRecv->iSvrCode;
			pMsg.iCurUserCount = aRecv->iCurUserCount;
			pMsg.iMaxUserCount = aRecv->iMaxUserCount;
			g_ConnecServerUDP.SendData((LPBYTE)&pMsg, pMsg.h.size);
		}else
		{
			PMSG_NOTIFY_USERS2 pMsg={0};
			PHeadSetB((LPBYTE)&pMsg, 0x04, sizeof(pMsg));
			pMsg.iCurUserCount  = aRecv->iCurUserCount;
			pMsg.iMaxUserCount  = aRecv->iMaxUserCount;
			pMsg.Port			= sManager->eObj[iIndex].m_wPort;
			g_ConnecServerUDP.SendData((LPBYTE)&pMsg, pMsg.h.size);
		}
	}
}

void NoticeRecv(cJoinServer * sManager, int aIndex, SDHP_NOTICE * aRecv)
{
	sManager->vLogAddC(5,"[NoticeRecv] :: %s",aRecv->Notice);

	for(int i = 0 ; i != g_MuLoginServerDataCount ; i++)
		GJNoticeSend(sManager,i,aRecv->Notice);

	//ObjDel(aIndex);
}

void MngPro_UserNoticeSend(cJoinServer * sManager, int aIndex,PMSG_USERNOTICE * lpMsg)
{
	BYTE result = 0x01;
	int  user=0;
	int  userInServer=0;

	if( strlen(lpMsg->Notice) < 1 ) 
		result = 0x00;

	if( strlen(lpMsg->AccountID) < 1 ) 
		result = 0x00;

	if( result == 0x01 )
	{		
		user = MuLoginFindUser(lpMsg->AccountID);
		if( user < 0 )
		{
			result = 0x00;
		}
		else
		{
			userInServer = g_MuLoginUserData[user].m_ServerCode;
		}
	}

	if( result == 0x01 )
	{
		SDHP_USER_NOTICE  pMsg={0};
		PHeadSetB((LPBYTE)&pMsg, 0x41, sizeof(pMsg));
		strcpy(pMsg.szId, lpMsg->AccountID);
		strcpy(pMsg.Notice, lpMsg->Notice);
		sManager->DataSend(userInServer, (LPBYTE)&pMsg, pMsg.h.size);
	}

	PMSG_USERNOTICE_RESULT pMsg={0};
	PHeadSetB((LPBYTE)&pMsg, 0xA2, sizeof(pMsg));
	pMsg.ClientIndex= lpMsg->ClientIndex;
	pMsg.Result		= result;
	sManager->DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}


void MngPro_GameBlock(cJoinServer * sManager, int aIndex,PMSG_GAME_BLOCK * lpMsg)
{
	int  user=0;
	int  userInServer=0;

	if( strlen( lpMsg->AccountID ) <= 0 || strlen( lpMsg->CharName ) <= 0 )
	{
		return;
	}

	user = MuLoginFindUser(lpMsg->AccountID);

	if( user < 0 )
	{
		return;
	}

	userInServer = g_MuLoginUserData[user].m_ServerCode;	
	
	PMSG_GAME_BLOCK pMsg={0};
	
	PHeadSetB((LPBYTE)&pMsg, 0x42, sizeof(pMsg));
	strcpy(pMsg.AccountID, lpMsg->AccountID);
	strcpy(pMsg.CharName, lpMsg->CharName);

	pMsg.ClientIndex = lpMsg->ClientIndex;
	pMsg.ServerNum   = lpMsg->ServerNum;
	pMsg.Type		 = lpMsg->Type;

	sManager->DataSend(userInServer, (LPBYTE)&pMsg, pMsg.h.size);
}