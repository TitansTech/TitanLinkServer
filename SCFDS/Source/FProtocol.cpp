#include "stdafx.h"
#include "config.h"
#include "winutil.h"
#include "ExDBProtocol.h"
#include "FProtocol.h"
#include <queue>
#include "../stdafx.h"

using namespace std;

queue<LPREQUEST_CMD> g_FriendQueue;
//CRITICAL_SECTION g_csFriend;
TSynchroCriticalSection Sync_csFriend;

queue<LPREQUEST_CMD> g_MailQueue;
TSynchroCriticalSection Sync_csMail;
//CRITICAL_SECTION g_csMail;

BOOL g_bExit = FALSE;
HANDLE hFriendThread, hMailThread;


int g_iMaxFriends = 50;

#pragma pack(1)

typedef struct _NAME
{
	char m_szName[11];
	_NAME()
	{
		ZeroMemory(this, sizeof(*this));
	}
	_NAME(char *szName)
	{
		ZeroMemory(this, sizeof(*this));
		strncpy(m_szName, szName, 10);
	}
}NAME, LPNAME;

typedef struct _FRIEND_MASTER
{
	int m_iGUID;
	int m_iMemoTotal;
	int m_iNumber;
	BYTE m_btState;
	int m_iServer;
	vector<NAME> m_vecFriends;

	_FRIEND_MASTER()
	{
		m_iGUID = -1;
		m_iMemoTotal = 0;
		m_iNumber = -1;
		m_btState = -1;
		m_iServer = -1;
		m_vecFriends.clear();
	}
}FRIEND_MASTER, *LPFRIEND_MASTER;
typedef map<string, FRIEND_MASTER, strCmp> MAP_FRIEND_MASTER;

//CRITICAL_SECTION g_csMapFriendMaster;
TSynchroCriticalSection Sync_csMapFriendMaster;

MAP_FRIEND_MASTER g_mapFriendManager;

#pragma pack()


BOOL CreateFriendMaster(char *szMaster);
BOOL DeleteFriendMaster(char *szMaster);


// use only for FriendProtocolCore
LPFRIEND_MASTER GetFriendMaster(char *szMaster)
{
	if(szMaster == NULL)
		return NULL;

	if(strlen(szMaster) < 4)
		return NULL;

	MAP_FRIEND_MASTER::iterator it;
	it = g_mapFriendManager.find(szMaster);
	if(it == g_mapFriendManager.end())
		return NULL;

	return &it->second;
}

BOOL FriendExists(char *szMaster, char *szFriend)
{
	LPFRIEND_MASTER lpMaster;

	//EnterCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Lock();
	//OutputDebugString("FriendExists=EnterCriticalSection=g_csMapFriendMaster\n");
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
	{
		Sync_csMapFriendMaster.Unlock();
		//LeaveCriticalSection(&g_csMapFriendMaster);	//Call 1
		//OutputDebugString("FriendExists=LeaveCriticalSection=g_csMapFriendMaster\n");
		return FALSE;
	}

	int size = lpMaster->m_vecFriends.size();
	int i = 0;

	for(i=0; i < size; i++)
	{
		if(strcmp(lpMaster->m_vecFriends[i].m_szName, szFriend) == 0)
			break;
	}

	//LeaveCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Unlock();
	//OutputDebugString("FriendExists=LeaveCriticalSection=g_csMapFriendMaster\n");
	if(i == size)
		return FALSE;
	else
		return TRUE;
}


BOOL CreateFriendMaster(char *szMaster, int iNumber, int iServer)
{
	LPFRIEND_MASTER lpM;
	lpM = GetFriendMaster(szMaster);
	if(lpM)
	{
		lpM->m_iNumber = iNumber;
		lpM->m_iServer = iServer;
		lpM->m_btState = iServer;
		return TRUE;
	}

	FRIEND_MASTER Master;
	Master.m_iNumber = iNumber;
	Master.m_iServer = iServer;
	Master.m_btState = iServer;

	//EnterCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Lock();
	//OutputDebugString("CreateFriendMaster=EnterCriticalSection=g_csMapFriendMaster\n");
	g_mapFriendManager[szMaster] = Master;
	//LeaveCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Unlock();
	//OutputDebugString("CreateFriendMaster=LeaveCriticalSection=g_csMapFriendMaster\n");

	return TRUE;
}

BOOL DeleteFriendMaster(char *szMaster)
{
	MAP_FRIEND_MASTER::iterator it;

	//EnterCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Lock();
	//OutputDebugString("DeleteFriendMaster=EnterCriticalSection=g_csMapFriendMaster\n");
	it = g_mapFriendManager.find(szMaster);
	if(it == g_mapFriendManager.end())
	{
		Sync_csMapFriendMaster.Unlock();
		//LeaveCriticalSection(&g_csMapFriendMaster);	//Call 1
		//OutputDebugString("DeleteFriendMaster=LeaveCriticalSection=g_csMapFriendMaster\n");
		return FALSE;
	}

	g_mapFriendManager.erase(it);
	Sync_csMapFriendMaster.Unlock();
	//LeaveCriticalSection(&g_csMapFriendMaster);	//Call 1
	//OutputDebugString("DeleteFriendMaster=LeaveCriticalSection=g_csMapFriendMaster\n");

	return TRUE;
		
}

int GetFriendGuid(char *szMaster)
{
	int guid = -1;
	LPFRIEND_MASTER lpMaster;
	//EnterCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Lock();
	//OutputDebugString("GetFriendGuid=EnterCriticalSection=g_csMapFriendMaster\n");

	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster)
		guid = lpMaster->m_iGUID;
	//LeaveCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Unlock();
	//OutputDebugString("GetFriendGuid=LeaveCriticalSection=g_csMapFriendMaster\n");

	return guid;
}

BYTE GetFriendState(char *szMaster)
{
	BYTE btState = -1;
	LPFRIEND_MASTER lpMaster;

	//EnterCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Lock();
	//OutputDebugString("GetFriendState=EnterCriticalSection=g_csMapFriendMaster\n");

	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster)
		btState = lpMaster->m_btState;
	//LeaveCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Unlock();
	//OutputDebugString("GetFriendState=LeaveCriticalSection=g_csMapFriendMaster\n");

	return btState;
}

int GetFriendServer(char *szMaster)
{
	int iServer = -1;
	LPFRIEND_MASTER lpMaster;

	//EnterCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Lock();
	//OutputDebugString("GetFriendServer=EnterCriticalSection=g_csMapFriendMaster\n");

	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster)
		iServer = lpMaster->m_iServer;
	Sync_csMapFriendMaster.Unlock();
	//LeaveCriticalSection(&g_csMapFriendMaster);	//Call 1
	//OutputDebugString("GetFriendServer=LeaveCriticalSection=g_csMapFriendMaster\n");

	return iServer;
}

int GetFriendNumber(char *szMaster)
{
	int iNumber = -1;
	LPFRIEND_MASTER lpMaster;

	//EnterCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Lock();
	//OutputDebugString("GetFriendNumber=EnterCriticalSection=g_csMapFriendMaster\n");

	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster)
		iNumber = lpMaster->m_iNumber;
	//LeaveCriticalSection(&g_csMapFriendMaster);	//Call 1
	Sync_csMapFriendMaster.Unlock();
	//OutputDebugString("GetFriendNumber=LeaveCriticalSection=g_csMapFriendMaster\n");

	return iNumber;
}


BOOL AddReqToFriendQueue(cWzExDBServer * sManager, int aIndex, LPBYTE lpReq)
{
	if(g_bExit)
		return FALSE;

	if(lpReq == NULL)
		return FALSE;

	LPREQUEST_CMD lpCMD;

	lpCMD = new REQUEST_CMD;
	if(lpCMD == NULL)
	{
		sManager->vLogAddC(2, "[ExDB]Memory Allocation Error!");
		return FALSE;
	}

	int size;
	if(((PMSG_DEFAULT *)lpReq)->h.c == 0xC1)
		size = ((PMSG_DEFAULT *)lpReq)->h.size;
	else
		size = MAKE_NUMBERW(((PWMSG_HEAD *)lpReq)->sizeL, ((PWMSG_HEAD *)lpReq)->sizeH);

	memcpy(lpCMD->m_btData, lpReq, size);
	lpCMD->m_aIndex = aIndex;

	//EnterCriticalSection(&g_csFriend);	//Call 1
	Sync_csFriend.Lock();
	//OutputDebugString("AddReqToFriendQueue=EnterCriticalSection=g_csFriend\n");

	g_FriendQueue.push(lpCMD);

	//LeaveCriticalSection(&g_csFriend);	//Call 1
	Sync_csFriend.Unlock();
	//OutputDebugString("AddReqToFriendQueue=LeaveCriticalSection=g_csFriend\n");

	return TRUE;
}

LPREQUEST_CMD GetFriendReq()
{
	LPREQUEST_CMD lpCmd = NULL;
	//EnterCriticalSection(&g_csFriend);	//Call 1
	Sync_csFriend.Lock();
	//OutputDebugString("GetFriendReq=EnterCriticalSection=g_csFriend\n");

	if(!g_FriendQueue.empty())
	{
		lpCmd = g_FriendQueue.front();
		g_FriendQueue.pop();
	}

	//LeaveCriticalSection(&g_csFriend);	//Call 1
	Sync_csFriend.Unlock();
	//OutputDebugString("GetFriendReq=LeaveCriticalSection=g_csFriend\n");

	return lpCmd;
}

void ClearFriendQueue()
{
	LPREQUEST_CMD lpCmd = NULL;
	//EnterCriticalSection(&g_csFriend);	//Call 1
	Sync_csFriend.Lock();
	//OutputDebugString("ClearFriendQueue=EnterCriticalSection=g_csFriend\n");

	try
	{
		while(!g_FriendQueue.empty())
		{
			lpCmd = g_FriendQueue.front();
			g_FriendQueue.pop();
			if(lpCmd)
				delete [] lpCmd;
		}
	}catch(...)
	{
	}

	//LeaveCriticalSection(&g_csFriend);	//Call 1
	Sync_csFriend.Unlock();
	//OutputDebugString("ClearFriendQueue=LeaveCriticalSection=g_csFriend\n");
}

BOOL AddReqToMailQueue(cWzExDBServer * sManager, int aIndex, LPBYTE lpReq)
{
	if(g_bExit)
		return FALSE;

	if(lpReq == NULL)
		return FALSE;

	LPREQUEST_CMD lpCMD;

	lpCMD = new REQUEST_CMD;
	if(lpCMD == NULL)
	{
		sManager->vLogAddC(2, "[ExDB]Memory Allocation Error!");
		return FALSE;
	}

	int size;
	if(((PMSG_DEFAULT *)lpReq)->h.c == 0xC1)
		size = ((PMSG_DEFAULT *)lpReq)->h.size;
	else
		size = MAKEWORD(((PWMSG_HEAD *)lpReq)->sizeL, ((PWMSG_HEAD *)lpReq)->sizeH);

	memcpy(lpCMD->m_btData, lpReq, size);
	lpCMD->m_aIndex = aIndex;


	//EnterCriticalSection(&g_csMail);	//Call 1
	Sync_csMail.Lock();
	//OutputDebugString("AddReqToMailQueue=EnterCriticalSection=g_csMail\n");

	g_MailQueue.push(lpCMD);

	//LeaveCriticalSection(&g_csMail);	//Call 1
	Sync_csMail.Unlock();
	//OutputDebugString("AddReqToMailQueue=LeaveCriticalSection=g_csMail\n");

	return TRUE;
}

LPREQUEST_CMD GetMailReq()
{
	LPREQUEST_CMD lpCmd = NULL;
	//EnterCriticalSection(&g_csMail);	//Call 1
	Sync_csMail.Lock();
	//OutputDebugString("GetMailReq=EnterCriticalSection=g_csMail\n");

	if(!g_MailQueue.empty())
	{
		lpCmd = g_MailQueue.front();
		g_MailQueue.pop();
	}

	//LeaveCriticalSection(&g_csMail);	//Call 1
	Sync_csMail.Unlock();
	//OutputDebugString("GetMailReq=LeaveCriticalSection=g_csMail\n");

	return lpCmd;
}

void ClearMailQueue()
{
	LPREQUEST_CMD lpCmd = NULL;

	//EnterCriticalSection(&g_csMail);	//Call 1
	Sync_csMail.Lock();
	//OutputDebugString("ClearMailQueue=EnterCriticalSection=g_csMail\n");

	while(!g_MailQueue.empty())
	{
		lpCmd = g_MailQueue.front();
		g_MailQueue.pop();
		if(lpCmd)
			delete [] lpCmd;
	}

	//LeaveCriticalSection(&g_csMail);	//Call 1
	Sync_csMail.Unlock();
	//OutputDebugString("ClearMailQueue=LeaveCriticalSection=g_csMail\n");
}

BOOL InitFriendServer(cWzExDBServer * sManager)
{
	//InitializeCriticalSection(&g_csMapFriendMaster);
	//OutputDebugString("InitFriendServer=InitializeCriticalSection=g_csMapFriendMaster\n");
	//InitializeCriticalSection(&g_csFriend);
	//OutputDebugString("InitFriendServer=InitializeCriticalSection=g_csFriend\n");
	//InitializeCriticalSection(&g_csMail);
	//OutputDebugString("InitFriendServer=InitializeCriticalSection=g_csMail\n");

	DWORD threadID;
	hFriendThread = CreateThread(NULL, 0, &FriendProtocolCore, sManager, 0, &threadID );
	hMailThread = CreateThread(NULL, 0, &MailProtocolCore, sManager, 0, &threadID );
	return TRUE;
}

void CloseFriendServer()
{

	g_bExit = TRUE;

	ClearFriendQueue();
	ClearMailQueue();

	if(WaitForSingleObject(hFriendThread, 200) == WAIT_TIMEOUT)
		TerminateThread(hFriendThread, 0);

	if(WaitForSingleObject(hMailThread, 200) == WAIT_TIMEOUT)
		TerminateThread(hMailThread, 0);


	//DeleteCriticalSection(&g_csMail);
	//OutputDebugString("CloseFriendServer=DeleteCriticalSection=g_csMail\n");
	//DeleteCriticalSection(&g_csFriend);
	//OutputDebugString("CloseFriendServer=DeleteCriticalSection=g_csFriend\n");
	//DeleteCriticalSection(&g_csMapFriendMaster);
	//OutputDebugString("CloseFriendServer=DeleteCriticalSection=g_csMapFriendMaster\n");
}

DWORD __stdcall FriendProtocolCore(void *pArg)
{
	cWzExDBServer *	sManager = (cWzExDBServer *)pArg;

	LPREQUEST_CMD lpCmd;
	do
	{
		lpCmd = (LPREQUEST_CMD)GetFriendReq();
		if(lpCmd)
		{
			BYTE headcode;
			if(((PMSG_DEFAULT *)&lpCmd->m_btData)->h.c == 0xC1)
				headcode = ((PMSG_DEFAULT *)&lpCmd->m_btData)->h.headcode;
			else
				headcode = ((PWMSG_HEAD *)&lpCmd->m_btData)->headcode;

			switch( headcode )
			{
				case 0x02:
					FriendClose(sManager, lpCmd->m_aIndex, (LPBYTE)&lpCmd->m_btData);
				break;

				case 0x60:
					FriendListRequest(sManager,  lpCmd->m_aIndex, (FHP_FRIENDLIST_REQ*)&lpCmd->m_btData);
				break;
				
				case 0x62:
					FriendStateClientRecv(sManager,  lpCmd->m_aIndex, (FHP_FRIEND_STATE_C*)&lpCmd->m_btData);
				break;

				case 0x63:
					FriendAddRequest(sManager,  lpCmd->m_aIndex, (FHP_FRIEND_ADD_REQ*)&lpCmd->m_btData);
				break;

				case 0x64:
					WaitFriendAddRequest(sManager,  lpCmd->m_aIndex, (FHP_WAITFRIEND_ADD_REQ*)&lpCmd->m_btData);
				break;

				case 0x65:
					FriendDelRequest(sManager,  lpCmd->m_aIndex, (FHP_FRIEND_ADD_REQ*)&lpCmd->m_btData);
				break;
			}


			delete [] lpCmd;
		}

		
		Sleep(1);

	}while(g_bExit != TRUE);

	return 0;
}

DWORD __stdcall MailProtocolCore(void *pArg)
{
	cWzExDBServer *	sManager = (cWzExDBServer *)pArg;
	LPREQUEST_CMD lpCmd;
	do
	{
		lpCmd = (LPREQUEST_CMD)GetMailReq();
		if(lpCmd)
		{
			BYTE headcode;
			if(((PMSG_DEFAULT *)&lpCmd->m_btData)->h.c == 0xC1)
				headcode = ((PMSG_DEFAULT *)&lpCmd->m_btData)->h.headcode;
			else
				headcode = ((PWMSG_HEAD *)&lpCmd->m_btData)->headcode;

			switch( headcode )
			{

				case 0x70:
					FriendMemoSend(sManager, lpCmd->m_aIndex, (FHP_FRIEND_MEMO_SEND*)&lpCmd->m_btData);
				break;

				case 0x71:
					FriendMemoListReq(sManager, lpCmd->m_aIndex, (FHP_FRIEND_MEMO_LIST_REQ*)&lpCmd->m_btData);
				break;

				case 0x72:
					FriendMemoReadReq(sManager, lpCmd->m_aIndex, (FHP_FRIEND_MEMO_RECV_REQ*)&lpCmd->m_btData);
				break;

				case 0x73:
					FriendMemoDelReq(sManager, lpCmd->m_aIndex, (FHP_FRIEND_MEMO_DEL_REQ*)&lpCmd->m_btData);
				break;
			}

			delete [] lpCmd;
		}

		
		Sleep(1);

	}while(g_bExit != TRUE);

	return 0;
}


int ExDBGetIndexByCode(cWzExDBServer * sManager, int iServerCode)
{
	if(iServerCode < 0)
		return -1;
	for(int i=0;i<sManager->MaxObj;i++)
	{
		if(sManager->isConnected(i) == true && sManager->eObj[i].m_State == SS_GUILD && sManager->eObj[i].m_ServerCode == iServerCode )
		{
			return i;
		}
	}

	return -1;
}

void FriendClose(cWzExDBServer * sManager, int aIndex, LPBYTE lpMsg)
{
	char szMaster[11] = {0};

	strncpy(szMaster, ((SDHP_USERCLOSE *)lpMsg)->CharName, 10);
	
	LPFRIEND_MASTER lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return;

	lpMaster->m_btState = -1;
	SendStateToAllFriends(sManager,aIndex, szMaster);
	DeleteFriendMaster(szMaster);
}



BOOL UserGuidDBCreate(cWzExDBServer * sManager, char *szName)
{
	//Open Call 1
	BOOL res = sManager->db.ExecQuery("WZ_UserGuidCreate '%s'", szName);
	sManager->db.Fetch();
	sManager->db.Close();	//Close Call 1

	return res;
}

BOOL GetDBGuidAndMemoTotal(cWzExDBServer * sManager, char *szName, int& guid, int& memo_total)
{
	BOOL res = FALSE;

	//Open Call 1
	if(sManager->db.ExecQuery("SELECT GUID, MemoTotal FROM T_FriendMain where Name = '%s'", szName))
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			guid = sManager->db.GetAsInteger("GUID");
			memo_total = sManager->db.GetAsInteger("MemoTotal");
			res = TRUE;
		}
	}
	sManager->db.Close();	//Close Call 1

	return res;
}



BOOL GetDBFriendList(cWzExDBServer * sManager, char *szMaster)
{
	LPFRIEND_MASTER lpMaster;

	BOOL res = TRUE;
	lpMaster = GetFriendMaster(szMaster);
	if(szMaster == NULL)
		return FALSE;

	if(GetDBGuidAndMemoTotal(sManager,szMaster, lpMaster->m_iGUID, lpMaster->m_iMemoTotal) == FALSE)
	{
		res = FALSE;
		if(UserGuidDBCreate(sManager, szMaster))
		{
			res = GetDBGuidAndMemoTotal(sManager, szMaster, lpMaster->m_iGUID, lpMaster->m_iMemoTotal);
		}
	}

	if(res == FALSE)
		return FALSE;

	res = FALSE;
	int i=0;
	char szFriend[11] = {0};

	//Open Call 1
	if(sManager->db.ExecQuery("SELECT FriendGuid, FriendName, Del FROM T_FriendList where GUID = %d", lpMaster->m_iGUID))
	{
		while(sManager->db.Fetch() != SQL_NO_DATA)
		{
			if(i == g_iMaxFriends)
			{
				sManager->vLogAddC(2, "[ExDB]DB contains more then %d friends for [%s] character", g_iMaxFriends, szMaster);
				break;
			}

			sManager->db.GetAsString("FriendName", szFriend, 10);
			if(FriendExists(szMaster, szFriend) == FALSE)
			{
				lpMaster->m_vecFriends.push_back(szFriend);
				i++;
			}
		}
	}
	sManager->db.Close();	//Close Call 1

	return res;
}

void FriendListSend(cWzExDBServer * sManager, int aIndex, char *szMaster)
{
	LPFRIEND_MASTER lpMaster;

	sManager->vLogAddTD("[ExDB][Friend List Send] Name[%s].", szMaster);
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
	{
		sManager->vLogAddTD("[ExDB][Friend List Send] FrienMaster[%s] not found.", szMaster);
		return;
	}

	FHP_FRIENDLIST_COUNT* lpFriendListCnt;
	FHP_FRIENDLIST* lpFriendList;

	int cnt = lpMaster->m_vecFriends.size();
	int size = sizeof(*lpFriendListCnt)+sizeof(*lpFriendList)*cnt;
	LPBYTE lpData = new BYTE[size];
	ZeroMemory(lpData, size);

	lpFriendListCnt = (FHP_FRIENDLIST_COUNT*)lpData;
	lpFriendList = (FHP_FRIENDLIST*)(lpData+sizeof(*lpFriendListCnt));
	PHeadSetW(lpData, 0x60, size);
	lpFriendListCnt->Count = cnt;
	lpFriendListCnt->MailCount = lpMaster->m_iMemoTotal;
	lpFriendListCnt->Number = lpMaster->m_iNumber;
	strncpy(lpFriendListCnt->Name, szMaster, 10);
	
	sManager->vLogAddTD("[ExDB][Friend List Send] Friend Count[%d].", cnt);
	for(int i=0; i < cnt; i++)
	{
		LPFRIEND_MASTER lpFr;
		int iServ = -1;
		lpFr = GetFriendMaster(lpMaster->m_vecFriends[i].m_szName);
		if(lpFr)
		{
			// check if friendship is established
			if(FriendExists(lpMaster->m_vecFriends[i].m_szName, szMaster))
				iServ = lpFr->m_btState;
		}

		lpFriendList[i].Server = iServ;
		strncpy(lpFriendList[i].Name, lpMaster->m_vecFriends[i].m_szName, 10);
		
		char szFriend[11] = {0};
		strncpy(szFriend, lpFriendList[i].Name, 10);
		sManager->vLogAddTD("[ExDB][Friend List Send] Friend[%s] send.", szFriend);
	}

	sManager->DataSend(aIndex, lpData, size);

	delete [] lpData;
}

BOOL GetDBWaitFriend(cWzExDBServer * sManager, char *szMaster, OUT char *szWaitFriend)
{
	LPFRIEND_MASTER lpMaster;
	lpMaster = GetFriendMaster(szMaster);

	if(lpMaster == NULL)
		return FALSE;

	BOOL res = FALSE;

	//Open Call 1
	if(sManager->db.ExecQuery("SELECT FriendName FROM T_WaitFriend where GUID=%d", lpMaster->m_iGUID))
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			sManager->db.GetAsString("FriendName", szWaitFriend, 10);
			res = TRUE;
		}
	}
	sManager->db.Close();	//Close Call 1

	return res;
}

BOOL FriendWaitSend(cWzExDBServer * sManager, int aIndex, char *szMaster)
{
	FHP_WAITFRIENDLIST_COUNT WaitFr;
	ZeroMemory(&WaitFr, sizeof(WaitFr));
	PHeadSetB((LPBYTE)&WaitFr, 0x61, sizeof(WaitFr));

	char szWaitFriend[11] = {0};

	LPFRIEND_MASTER lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return FALSE;

	if(GetDBWaitFriend(sManager,szMaster, szWaitFriend) == FALSE)
		return FALSE;

	strncpy(WaitFr.FriendName, szWaitFriend, 10);
	strncpy(WaitFr.Name, szMaster, 10);
	WaitFr.Number = lpMaster->m_iNumber;

	sManager->DataSend(aIndex, (LPBYTE)&WaitFr, WaitFr.h.size);

	return TRUE;
}


void SendState(cWzExDBServer * sManager, int aIndex, char *szMaster, int iNumber, char *szName, BYTE btState)
{
	FHP_FRIEND_STATE State;
	ZeroMemory(&State, sizeof(State));

	PHeadSetB((LPBYTE)&State, 0x62, sizeof(State));

	strncpy(State.Name, szMaster, 10);
	strncpy(State.FriendName, szName, 10);
	State.Number = iNumber;
	State.State = btState;

	sManager->DataSend(aIndex, (LPBYTE)&State, State.h.size);
}

void SendStateToAllFriends(cWzExDBServer * sManager, int aIndex, char *szMaster)
{
	LPFRIEND_MASTER lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return;

	int cnt = lpMaster->m_vecFriends.size();

	for(int i=0; i < cnt; i++)
	{
		LPFRIEND_MASTER lpFr;
		lpFr = GetFriendMaster(lpMaster->m_vecFriends[i].m_szName);
		if(lpFr)
		{
			// check if friendship is established
			if(FriendExists(lpMaster->m_vecFriends[i].m_szName, szMaster))
			{
				int index = ExDBGetIndexByCode(sManager,lpFr->m_iServer);
				if(index != -1)
				{
					SendState(sManager,index, 
						lpMaster->m_vecFriends[i].m_szName, 
						lpFr->m_iNumber, szMaster, lpMaster->m_btState);
				}
			}
		}
	}
}


void FriendListRequest(cWzExDBServer * sManager, int aIndex, FHP_FRIENDLIST_REQ* lpMsg)
{
	char szMaster[11]={0};
	strncpy(szMaster, lpMsg->Name, 10);

	sManager->vLogAddTD("[ExDB][Friend List Request] Name[%s].", szMaster);
	if(CreateFriendMaster(szMaster, lpMsg->Number, lpMsg->pServer) == FALSE)
		return;

	GetDBFriendList(sManager,szMaster);
	FriendListSend(sManager,aIndex, szMaster);
	FriendWaitSend(sManager,aIndex, szMaster);
	SendStateToAllFriends(sManager,aIndex, szMaster);

	FHP_FRIEND_MEMO_LIST_REQ req;
	ZeroMemory(&req, sizeof(req));
	PHeadSetB((LPBYTE)&req, 0x71, sizeof(req));

	strncpy(req.Name, lpMsg->Name, 10);
	req.Number = lpMsg->Number;

	AddReqToMailQueue(sManager,aIndex, (LPBYTE)&req);
}


BOOL AddFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend)
{
	LPFRIEND_MASTER lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return FALSE;

	if( lpMaster->m_vecFriends.size() == g_iMaxFriends)
	{
		sManager->vLogAddC(0, "[ExDB][%s]'s friend list is full - %d", szMaster, g_iMaxFriends);
		return FALSE;
	}

	lpMaster->m_vecFriends.push_back(szFriend);

	sManager->vLogAddTD("[ExDB][%s] became [%s]'s friend.", szFriend, szMaster);
	return TRUE;
}

int AddDBWaitFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend)
{
	int res = 1;

	//Open Call 1
	if(sManager->db.ExecQuery("EXEC WZ_WaitFriendAdd '%s', '%s'", szMaster, szFriend))
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			res = sManager->db.GetResult(0);
			if(res != 0)
			{
				if(res == 6)
					sManager->vLogAdd("[ExDB][%s]'s level is too low.", szFriend);
				else if(res == 3)
					sManager->vLogAddTD("[ExDB]GUID for [%s] doesn't exists.", szFriend);
				else if(res != 8)
					sManager->vLogAddC(2, "[ExDB]WZ_WaitFriendAdd returned: %d", res);
			}
		}
		else
			sManager->vLogAddC(2, "[ExDB]WZ_WaitFriendAdd no result returned.");
	}
	else
	{
		sManager->vLogAddC(2, "[ExDB]Error executing WZ_WaitFriendAdd.");
	}
	sManager->db.Close();	//Close Call 1

	return res;
}


void FriendAddRequest(cWzExDBServer * sManager,  int aIndex, FHP_FRIEND_ADD_REQ* lpMsg)
{
	FHP_FRIEND_ADD_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));
	PHeadSetB((LPBYTE)&Result, 0x63, sizeof(Result));

	strncpy(Result.Name, lpMsg->Name, 10);
	strncpy(Result.FriendName, lpMsg->FriendName, 10);
	Result.Result = 2;
	Result.Server = -1;
	Result.Number = lpMsg->Number;

	char szMaster[11] = {0};
	char szFriend[11] = {0};
	strncpy(szMaster, Result.Name, 10);
	strncpy(szFriend, Result.FriendName, 10);

	sManager->vLogAdd("[ExDB][Friend Add Request] Name[%s] FriendName[%s].", szMaster, szFriend);

	if(strcmp(szMaster, szFriend) == 0)
	{
		// own id
		Result.Result = 5;

		sManager->vLogAddTD("[ExDB][Friend Add Request] Send Result [%d].", Result.Result);
		sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
		return;
	}

	LPFRIEND_MASTER lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
	{
		sManager->vLogAdd("[ExDB][Friend Add Request] Send Result [%d].", Result.Result);
		sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
		return;
	}

	if(FriendExists(szMaster, szFriend))
	{
		Result.Result = 4;

		sManager->vLogAdd("[ExDB][Friend Add Request] Send Result [%d].", Result.Result);
		sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
		return;
	}

	if(lpMaster->m_vecFriends.size() == g_iMaxFriends)
	{
		Result.Result = 3;

		sManager->vLogAddTD("[ExDB][Friend Add Request] Send Result [%d].", Result.Result);
		sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
		return;
	}

	int res = AddDBWaitFriend(sManager,szMaster, szFriend);
	
	// DB result, GUID doesn't exists, or select error
	if(res > 1 && res < 6)
	{
		Result.Result = 0;

		sManager->vLogAddTD("[ExDB][Friend Add Request] Send Result [%d].", Result.Result);
		sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
		return;
	}

	// DB result, lvl< 6
	if(res == 6)
	{
		Result.Result = 6;
		sManager->vLogAddTD("[ExDB][Friend Add Request] Send Result [%d].", Result.Result);
		sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
		return;
	}

	// DB, other errors
	if(res != 0 && res != 8)
	{
		Result.Result = 2;
		sManager->vLogAddTD("[ExDB][Friend Add Request] Send Result [%d].", Result.Result);
		sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
		return;
	}

	LPFRIEND_MASTER lpFr;
	lpFr = GetFriendMaster(szFriend);

	Result.Server = -1;
	BYTE btState = 0;
	if(lpFr)
	{
		// check if friendship is established
		if(FriendExists(szFriend, szMaster))
		{
			Result.Server = lpFr->m_btState;
			SendState(sManager,aIndex, szFriend, lpFr->m_iNumber, szMaster, lpMaster->m_btState);
		}

		int index = ExDBGetIndexByCode(sManager,lpFr->m_iServer);
		if(index != -1)
			FriendWaitSend(sManager,aIndex, szFriend);
	}

	AddFriend(sManager,szMaster, szFriend);

	Result.Result = 1;

	sManager->vLogAddTD("[ExDB][Friend Add Request] Send Result [%d].", Result.Result);
	sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
}


int DelDBWaitFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend)
{
	int res = 1;

	//Open Call 1
	if(sManager->db.ExecQuery("EXEC WZ_WaitFriendDel '%s', '%s'", szMaster, szFriend))
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			res = sManager->db.GetResult(0);
			if(res != 0)
				sManager->vLogAddC(2, "[ExDB]WZ_WaitFriendDel returned: %d", res);
		}
		else
			sManager->vLogAddC(2, "[ExDB]WZ_WaitFriendDel no result returned.");
	}
	else
	{
		sManager->vLogAddC(2, "[ExDB]Error executing WZ_WaitFriendDel.");
	}

	sManager->db.Close();	//Close Call 1

	return res;
}


BOOL AddDBFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend)
{
	int res = -1;

	//Open Call 1
	if(sManager->db.ExecQuery("EXEC WZ_FriendAdd '%s', '%s'", szMaster, szFriend))
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			res = sManager->db.GetResult(0);
			if(res != 1)
				sManager->vLogAddC(2, "[ExDB]WZ_FriendAdd returned: %d", res);

		}
		else
			sManager->vLogAddC(2, "[ExDB]WZ_FriendAdd no result returned.");

	}
	else
	{
		sManager->vLogAddC(2, "[ExDB]Error executing WZ_FriendAdd.");
	}
	sManager->db.Close();	//Close Call 1

	if(res == 1)
		return TRUE;
	
	return FALSE;
}

BOOL DelDBFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend)
{
	int res = -1;

	//Open Call 1
	if(sManager->db.ExecQuery("EXEC WZ_FriendDel '%s', '%s'", szMaster, szFriend))
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			res = sManager->db.GetResult(0);
			if(res != 1)
				sManager->vLogAddC(2, "[ExDB]WZ_FriendDel returned: %d", res);

		}
		else
			sManager->vLogAddC(2, "[ExDB]WZ_FriendDel no result returned.");

	}
	else
	{
		sManager->vLogAddC(2, "[ExDB]Error executing WZ_FriendDel.");
	}
	sManager->db.Close();	//Close Call 1

	if(res == 1)
		return TRUE;
	
	return FALSE;
}


int WaitFriendAdd(cWzExDBServer * sManager, char *szMaster, char *szFriend)
{
	LPFRIEND_MASTER lpMaster;

	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return 0;

	if(FriendExists(szMaster, szFriend))
		return 4;

	if(lpMaster->m_vecFriends.size() == g_iMaxFriends)
	{
		sManager->vLogAddC(0, "[ExDB][%s]'s friend list is full - %d", szMaster, g_iMaxFriends);
		return 3;
	}

	if(AddDBFriend(sManager,szMaster, szFriend) == FALSE)
		return 0;

	if(AddFriend(sManager,szMaster, szFriend) == FALSE)
		return 0;

	LPFRIEND_MASTER lpFr;
	lpFr = GetFriendMaster(szMaster);
	if(lpFr)
	{
		int index = ExDBGetIndexByCode(sManager,lpFr->m_iServer);
		if(index != -1)
			SendState(sManager,index, szFriend, lpFr->m_iNumber, szMaster, lpMaster->m_btState);
	}

	return 1;
}

void WaitFriendAddRequest(cWzExDBServer * sManager,  int aIndex,  FHP_WAITFRIEND_ADD_REQ* lpMsg)
{
	FHP_WAITFRIEND_ADD_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));

	PHeadSetB((LPBYTE)&Result, 0x64, sizeof(Result));
	Result.Number = lpMsg->Number;

	strncpy(Result.Name, lpMsg->Name, 10);
	strncpy(Result.FriendName, lpMsg->FriendName, 10);
	Result.pServer = -1;
	
	char szMaster[11] = {0};
	char szFriend[11] = {0};

	strncpy(szMaster, lpMsg->Name, 10);
	strncpy(szFriend, lpMsg->FriendName, 10);

	sManager->vLogAddTD("[ExDB][WaitFriend Add Request] Name[%s], FriendName[%s].", szMaster, szFriend);

	if(lpMsg->Result == 0)
	{
		Result.Result = 2;
		sManager->vLogAddTD("[ExDB][WaitFriend Add Request] [%s] declined [%s]'s request to be a friend.", szMaster, szFriend);
		sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
		return;
	}

	sManager->vLogAddTD("[ExDB][WaitFriend Add Request] [%s] accepted [%s]'s request to be a friend.", szMaster, szFriend);

	LPFRIEND_MASTER lpFr;
	lpFr = GetFriendMaster(szFriend);
	if(lpFr)
		Result.pServer = lpFr->m_btState;

	Result.Result = WaitFriendAdd(sManager,szMaster, szFriend);
	if(Result.Result != 1)
		DelDBWaitFriend(sManager,szMaster, szFriend);


	FriendWaitSend(sManager,aIndex, szMaster);
	sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
}

void FriendStateClientRecv(cWzExDBServer * sManager, int aIndex, FHP_FRIEND_STATE_C* lpMsg)
{
	char szMaster[11] = {0};
	strncpy(szMaster, lpMsg->Name, 10);

	sManager->vLogAddTD("[ExDB][Friend State Receive] Name[%s], State[%d].", szMaster, lpMsg->State);
	LPFRIEND_MASTER lpMaster; 
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
	{
		sManager->vLogAddTD("[ExDB][Friend State Receive] error: FriendMaster[%s] not found.", szMaster);
		return;
	}

//	lpMaster->m_iNumber = lpMsg->Number;

	switch(lpMsg->State)
	{
		case 0:
			// refuse chat
			lpMaster->m_btState = -2;
		break;

		case 2:
			// ???, client shows "Cannot Use"
			lpMaster->m_btState = -3;
		break;

		default:
			// display server
			lpMaster->m_btState = lpMaster->m_iServer;
	}

	SendStateToAllFriends(sManager,aIndex, szMaster);
}

BOOL DelFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend)
{
	LPFRIEND_MASTER lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return FALSE;

	if(FriendExists(szMaster, szFriend) == FALSE)
		return FALSE;

	int cnt = lpMaster->m_vecFriends.size();
	for(int i=0; i < cnt; i++)
	{
		if(strcmp(lpMaster->m_vecFriends[i].m_szName, szFriend) == 0)
		{
			lpMaster->m_vecFriends.erase(lpMaster->m_vecFriends.begin()+i);
			sManager->vLogAddTD("[ExDB][%s] dropped [%s] from his friend list.", szMaster, szFriend);
			break;
		}

	}
	return TRUE;
}


void FriendDelRequest(cWzExDBServer * sManager,  int aIndex, FHP_FRIEND_ADD_REQ* lpMsg)
{
	FHP_FRIEND_DEL_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));

	PHeadSetB((LPBYTE)&Result, 0x65, sizeof(Result));

	Result.Number = lpMsg->Number;

	char szMaster[11] = {0};
	char szFriend[11] = {0};
	
	strncpy(Result.Name, lpMsg->Name, 10);
	strncpy(Result.FriendName, lpMsg->FriendName, 10);

	strncpy(szMaster, lpMsg->Name, 10);
	strncpy(szFriend, lpMsg->FriendName, 10);

	sManager->vLogAddTD("[ExDB][Friend Delete Request] Name[%s], FriendName[%s].", szMaster, szFriend);

	Result.Result = DelFriend(sManager,szMaster, szFriend);
	if(Result.Result)
		Result.Result = DelDBFriend(sManager,szMaster, szFriend);

	sManager->vLogAddTD("[ExDB][Friend Delete Request] Send Result [%d].", Result.Result);
	sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);

	LPFRIEND_MASTER lpFr;
	lpFr = GetFriendMaster(szFriend);
	if(lpFr)
	{
		int index = ExDBGetIndexByCode(sManager,lpFr->m_iServer);

		if(index != -1)
			SendState(sManager,index, szFriend, lpFr->m_iNumber, szMaster, -1);
	}
}


void MemoHeaderSend(cWzExDBServer * sManager, int aIndex, WORD wNumber, LPMEMO_HEADER lpMemoHead )
{
	FHP_FRIEND_MEMO_LIST MemoHead;
	ZeroMemory(&MemoHead, sizeof(MemoHead));

	PHeadSetW((LPBYTE)&MemoHead, 0x71, sizeof(MemoHead));
	MemoHead.Number = wNumber;
	memcpy(&(MemoHead.MemoIndex), lpMemoHead, sizeof(*lpMemoHead));

	sManager->DataSend(aIndex, (LPBYTE)&MemoHead, sizeof(MemoHead));
}

void MemoListSend(cWzExDBServer * sManager, int aIndex, WORD wNumber, char *szName)
{
	char szMaster[11] = {0};
	strncpy(szMaster, szName, 10);

	sManager->vLogAddTD("[ExDB][Mail List Send] Name[%s].", szMaster);
	int guid = GetFriendGuid(szMaster);
	if(guid == -1)
	{
		sManager->vLogAddTD("[ExDB][Mail List Send] error: GUID = -1.");
		return;
	}

	int cnt = 0;
	//Open Call 1
	if(sManager->db.ExecQuery("SELECT MemoIndex, FriendName, wDate, Subject, bRead FROM T_FriendMail where GUID=%d", guid))
	{
		while(sManager->db.Fetch() != SQL_NO_DATA)
		{
			MEMO_HEADER head;
			ZeroMemory(&head, sizeof(head));
			strncpy(head.RecvName, szName, 10);

			head.MemoIndex = sManager->db.GetAsInteger("MemoIndex");
			sManager->db.GetAsString("FriendName", head.SendName, 10);
			sManager->db.GetAsString("wDate", head.Date, 30);
			sManager->db.GetAsString("Subject", head.Subject, 32);
			head.read = sManager->db.GetAsInteger("bRead");
			MemoHeaderSend(sManager,aIndex, wNumber, &head);
			cnt++;
		}
	}
	sManager->db.Close();	//Close Call 1

	sManager->vLogAddTD("[ExDB][Mail List Send] Mail Count: %d.", cnt);
}


void FriendMemoListReq(cWzExDBServer * sManager,  int aIndex, FHP_FRIEND_MEMO_LIST_REQ* lpMsg)
{
	char szName[11] = {0};
	strncpy(szName, lpMsg->Name, 10);

	sManager->vLogAdd("[ExDB][Mail List Request] Name[%s].", szName);
	MemoListSend(sManager,aIndex, lpMsg->Number, szName);
}


int DBWriteMail(cWzExDBServer * sManager, LPMEMO_SEND_HEADER lpMemoSendHdr, LPBYTE Photo, BYTE btPhotoSize, char *sMemo, int memo_size)
{
	char szSendName[11] = {0};
	char szRecvName[11] = {0};

	strncpy(szSendName, lpMemoSendHdr->Name, 10);
	strncpy(szRecvName, lpMemoSendHdr->ToName, 10);

	//Open Call 1
	if(sManager->db.ExecQuery("EXEC WZ_WriteMail '%s', '%s', '%s', %d, %d", 
		szSendName, szRecvName, NULL, lpMemoSendHdr->Dir,  lpMemoSendHdr->Action))
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			int memo_index;
			int guid;

			memo_index = sManager->db.GetResult(0);
			guid = sManager->db.GetResult(1);
			sManager->db.Close();	//Close Call 1

			if(memo_index <= 10)
			{
				if(memo_index == 2 || memo_index == 3)
					return -3;

				if(memo_index == 5)
					return -2;

				if(memo_index == 6)
					return -6;

				sManager->vLogAddC(2, "[ExDB]WZ_WriteMail returned: %d", memo_index);
				return - 1;
			}

			if(guid < 0)
			{
				sManager->vLogAddC(2, "[ExDB]WZ_WriteMail returned guid: %d", guid);
				return - 1;
			}

			char szMsg[1500];

			//Open Call 2
			ZeroMemory(szMsg, 1500);
			wsprintf(szMsg, "UPDATE T_FriendMail SET Photo=? where MemoIndex=%d AND GUID=%d", memo_index, guid);
			sManager->db.SetAsBinary(szMsg, Photo, btPhotoSize);
			sManager->db.Close();	//Close Call 2

			//Open Call 3
			ZeroMemory(szMsg, 1500);
			wsprintf(szMsg, "UPDATE T_FriendMail SET Subject=? where MemoIndex=%d AND GUID=%d", memo_index, guid);
			sManager->db.SetAsBinary(szMsg, (LPBYTE)lpMemoSendHdr->Subject, 32);
			sManager->db.Close();	//Close Call 3

			//Open Call 4
			ZeroMemory(szMsg, 1500);
			wsprintf(szMsg, "UPDATE T_FriendMail SET Memo=? where MemoIndex=%d AND GUID=%d", memo_index, guid);
			sManager->db.SetAsBinary(szMsg, (LPBYTE)sMemo, memo_size);
			sManager->db.Close();	//Close Call 4

			return memo_index;
		}
		else
		{
			sManager->vLogAddC(2, "[ExDB]WZ_WriteMail no result returned.");
			sManager->db.Close();	//Close Call 1
		}
	}
	else
	{
		sManager->vLogAddC(2, "[ExDB]Error executing WZ_WriteMail");
		sManager->db.Close();	//Close Call 1
	}

	return -1;
}

void FriendMemoSend(cWzExDBServer * sManager, int aIndex, FHP_FRIEND_MEMO_SEND* lpMsg)
{
	FHP_FRIEND_MEMO_SEND_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));
	PHeadSetB((LPBYTE)&Result, 0x70, sizeof(Result));

	Result.Number = lpMsg->Number;
	Result.WindowGuid = lpMsg->WindowGuid;
	strncpy(Result.Name, lpMsg->Name, 10);
	
	char szName[11] = {0};
	strncpy(szName, Result.Name, 10);

	MEMO_SEND_HEADER head;
	ZeroMemory(&head, sizeof(head));

	memcpy(&head, lpMsg->Name, sizeof(head));
	
	int photo_size;
	int msg_size = MAKEWORD(lpMsg->h.sizeL, lpMsg->h.sizeH);
	LPBYTE lpMemo;
	LPBYTE lpPhoto;
	
	if(sizeof(*lpMsg) - sizeof(lpMsg->Memo) != msg_size - lpMsg->MemoSize)
	{
		photo_size = 13;
		lpMemo = (LPBYTE)((FHP_FRIEND_MEMO_SEND_OLD*)lpMsg)->Memo;
		lpPhoto = ((FHP_FRIEND_MEMO_SEND_OLD*)lpMsg)->Photo;
	}
	else
	{
		photo_size = 18;
		lpMemo = (LPBYTE)lpMsg->Memo;
		lpPhoto = lpMsg->Photo;

	}

	char szMaster[11]={0};
	strncpy(szMaster, lpMsg->ToName, 10);

	sManager->vLogAdd("[ExDB][Mail Send Request] Name[%s], ToName[%s], MailSize[%d], PhotoSize[%d].", 
		szName, szMaster, lpMsg->MemoSize, photo_size);

	int res = -1;

	if(lpMsg->MemoSize <= 1000)
		res = DBWriteMail(sManager,&head, lpPhoto, photo_size, (char *)lpMemo, lpMsg->MemoSize);
	else
		sManager->vLogAdd("[ExDB][Mail Send Request] error: Mail Size > 1000.");

	if(res == -2)
		Result.Result = 2;
	else if(res == -3)
		Result.Result = 3;
	else if(res == -6)
		Result.Result = 6;
	else if(res < 0)
		Result.Result = 0;
	else
		Result.Result = 1;

	sManager->vLogAdd("[ExDB][Mail Send Request] Send Result [%d].", Result.Result);
	sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
	
	if(Result.Result != 1)
		return;
	
	int iNumber = GetFriendNumber(szMaster);
	if(iNumber < 0 )
		return;

	MEMO_HEADER memo_head;
	memo_head.MemoIndex = res;
	memo_head.read = 0;
	strncpy(memo_head.Subject, lpMsg->Subject, 32);
	strncpy(memo_head.RecvName, lpMsg->ToName, 10);
	strncpy(memo_head.SendName, lpMsg->Name, 10);
	
	SYSTEMTIME st;
	GetLocalTime(&st);

	wsprintf(memo_head.Date, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);

	memo_head.read = 2;
	MemoHeaderSend(sManager,aIndex, iNumber, &memo_head);
}

BOOL DBReadMail(cWzExDBServer * sManager, int iMemoId, int iGuid, MEMO_READ & lpMemoRead, DWORD & lpdwMemoSize, LPBYTE lpPhoto, BYTE & lpPhotoSize)
{
	__try
	{
	//Open Call 1
	if(sManager->db.ExecQuery("SELECT bRead, Dir, Act FROM T_FriendMail where MemoIndex=%d AND GUID =%d", iMemoId, iGuid))
	{
		int bRead = 0;
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			bRead = sManager->db.GetAsInteger("bRead");
			lpMemoRead.Dir = sManager->db.GetAsInteger("Dir");
			lpMemoRead.Action = sManager->db.GetAsInteger("Act");
		}

		sManager->db.Close();	//Close Call 1

		char szMsg[1500]={0};
		//ZeroMemory(szMsg, 1500);

		//Open Call 2
		wsprintf(szMsg, "SELECT Photo FROM T_FriendMail where MemoIndex='%d' AND GUID=%d", iMemoId, iGuid);
		lpPhotoSize = sManager->db.GetAsBinary(szMsg, lpPhoto);
		sManager->db.Close();	//Close Call 2

		if(lpPhotoSize >= 18)
			lpPhotoSize = 18;
		else if(lpPhotoSize < 18)
			lpPhotoSize = 13;

		//Open Call 3
		//ZeroMemory(szMsg, 1500);
		wsprintf(szMsg, "SELECT Memo FROM T_FriendMail where MemoIndex='%d' AND GUID=%d", iMemoId, iGuid);
		lpdwMemoSize = sManager->db.GetAsBinary(szMsg, (LPBYTE)lpMemoRead.Memo);
		sManager->db.Close();	//Close Call 3

		if(lpdwMemoSize < 0)
			lpdwMemoSize = 0;
		if(bRead == 0)
		{
			//Open Call 4
			sManager->db.ExecQuery("UPDATE T_FriendMail SET bRead = 1 where MemoIndex=%d AND GUID=%d", iMemoId, iGuid);
			sManager->db.Fetch();
			sManager->db.Close();	//Close Call 4
		}

		return TRUE;
	}
	else
	{
		sManager->db.Close();	//Close Call 1
		return FALSE;
	}
	}__except( EXCEPTION_ACCESS_VIOLATION == GetExceptionCode() )
	{
		sManager->db.Close();	//Close Call 1
		return FALSE;
	}
}


void FriendMemoReadReq(cWzExDBServer * sManager,  int aIndex, FHP_FRIEND_MEMO_RECV_REQ* lpMsg)
{
	MEMO_READ memo;
	ZeroMemory(&memo, sizeof(memo));
	DWORD memo_size=0;
	BYTE Photo[30]={-1};
	BYTE btPhotoSize=18;
	char szMaster[11]={0};
	strncpy(szMaster, lpMsg->Name, 10);

	sManager->vLogAdd("[ExDB][Mail Read Request] Name[%s], MemoIndex[%d].", szMaster, lpMsg->MemoIndex);
	int guid = -1;
	guid = GetFriendGuid(szMaster);
	if(guid < 0)
	{
		sManager->vLogAdd("[ExDB][Mail Read Request] error: GUID = -1.");
		return;
	}

	if(DBReadMail(sManager,lpMsg->MemoIndex, guid, memo, memo_size, Photo, btPhotoSize))
	{
		if(btPhotoSize == 13)
		{
			FHP_FRIEND_MEMO_RECV_OLD Result;
			ZeroMemory(&Result, sizeof(Result));

			PHeadSetW((LPBYTE)&Result, 0x72, sizeof(Result)-sizeof(Result.Memo)+memo_size);

			Result.MemoIndex = lpMsg->MemoIndex;
			Result.Number = lpMsg->Number;
			strncpy(Result.Name, lpMsg->Name, 10);

			Result.MemoSize = memo_size;
			memcpy(&Result.Dir, &memo, sizeof(memo));
			memcpy(Result.Photo, Photo, 13);

			sManager->DataSend(aIndex, (LPBYTE)&Result, sizeof(Result)-sizeof(Result.Memo)+memo_size);
		}
		else
		{
			FHP_FRIEND_MEMO_RECV Result;
			ZeroMemory(&Result, sizeof(Result));

			PHeadSetW((LPBYTE)&Result, 0x72, sizeof(Result)-sizeof(Result.Memo)+memo_size);

			Result.MemoIndex = lpMsg->MemoIndex;
			Result.Number = lpMsg->Number;
			strncpy(Result.Name, lpMsg->Name, 10);

			Result.MemoSize = memo_size;
			memcpy(&Result.Dir, &memo, sizeof(memo));
			memcpy(Result.Photo, Photo, 18);

			sManager->DataSend(aIndex, (LPBYTE)&Result, sizeof(Result)-sizeof(Result.Memo)+memo_size);
		}

		sManager->vLogAdd("[ExDB][Mail Read Request] Send Mail: size[%d], photo size[%d].", memo_size, btPhotoSize);
	}
}

BOOL DBDelMail(cWzExDBServer * sManager, char *szName, int iMemoId)
{
	int res = FALSE;

	//Open Call 1
	if(sManager->db.ExecQuery("EXEC WZ_DelMail '%s', %d", szName, iMemoId))
	{
		if(sManager->db.Fetch() != SQL_NO_DATA)
		{
			res = sManager->db.GetResult(0);
			if(res != 1)
			{
				sManager->vLogAddC(2, "[ExDB]WZ_DelMail returned: %d", res);
				res = FALSE;
			}
			else
				res = TRUE;
		}
		else
			sManager->vLogAddC(2, "[ExDB]WZ_DelMail no result returned.");
	}
	else
	{
		sManager->vLogAddC(2, "[ExDB]Error executing WZ_DelMail");
	}
	sManager->db.Close();	//Close Call 1

	return res;
}

void FriendMemoDelReq( cWzExDBServer * sManager, int aIndex, FHP_FRIEND_MEMO_DEL_REQ* lpMsg)
{
	FHP_FRIEND_MEMO_DEL_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));
	PHeadSetB((LPBYTE)&Result, 0x73, sizeof(Result));

	Result.MemoIndex = lpMsg->MemoIndex;
	Result.Number = lpMsg->Number;
	strncpy(Result.Name, lpMsg->Name, 10);

	char szName[11]={0};
	strncpy(szName, lpMsg->Name, 10);

	sManager->vLogAdd("[ExDB][Mail Delete Request] Name[%s], MamoIndex[%d].", szName, lpMsg->MemoIndex);

	Result.Result = DBDelMail(sManager,szName, lpMsg->MemoIndex);

	sManager->vLogAdd("[ExDB][Mail Delete Request] Send Result [%d].", Result.Result);
	sManager->DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
}