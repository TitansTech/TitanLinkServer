
#include "ServerManager.h"

struct FHP_FRIENDLIST_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x6>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0x10>*/ /*|0x1|*/ unsigned char pServer;

};
// <size 0x12>

struct FHP_FRIENDLIST 
{

  /*<thisrel this+0x0>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0xa>*/ /*|0x1|*/ unsigned char Server;
};
// <size 0xb>

struct FHP_FRIENDLIST_COUNT 
{

  /*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x6>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0x10>*/ /*|0x1|*/ unsigned char Count;
  /*<thisrel this+0x11>*/ /*|0x1|*/ unsigned char MailCount;

};
// <size 0x12>


struct FHP_WAITFRIENDLIST_COUNT
{
	PBMSG_HEAD h;	//	
	short Number;	//	4
	char Name[10];	//	6
	char FriendName[10];	//	10
};

struct FHP_FRIEND_STATE_C 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x6>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0x10>*/ /*|0x1|*/ unsigned char State;
};
// <size 0x12>



struct FHP_FRIEND_STATE
{
	PBMSG_HEAD h;	//	
	short Number;	//	4
	char Name[10];	//	6
	char FriendName[10];	//	10
	unsigned char State;	//	1A
};


struct FHP_FRIEND_ADD_REQ 
{
  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x6>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0x10>*/ /*|0xa|*/ char FriendName[10];
};
// <size 0x1a>


struct FHP_FRIEND_ADD_RESULT
{
	PBMSG_HEAD h;
	short Number;	// 4
	unsigned char Result;	// 6
	char Name[10];	// 7
	char FriendName[10];	// 11
	unsigned char Server;	// 1B
};

struct FHP_WAITFRIEND_ADD_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x3>*/ /*|0x1|*/ unsigned char Result;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x6>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0x10>*/ /*|0xa|*/ char FriendName[10];
};
// <size 0x1a>


struct FHP_WAITFRIEND_ADD_RESULT
{
	PBMSG_HEAD h;	//	
	short Number;	//	4
	unsigned char Result;	//	6
	char Name[10];	//	7
	char FriendName[10];	//	11
	unsigned char pServer;	//	1B
};



struct FHP_FRIEND_DEL_RESULT
{
	PBMSG_HEAD h;	//	
	short Number;	//	4
	unsigned char Result;	//	6
	char Name[10];	//	7
	char FriendName[10];	//	11
};


struct FHP_FRIEND_MEMO_SEND 
{
  /*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x8>*/ /*|0x4|*/ unsigned long WindowGuid;
  /*<thisrel this+0xc>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0x16>*/ /*|0xa|*/ char ToName[10];
  /*<thisrel this+0x20>*/ /*|0x20|*/ char Subject[32];
  /*<thisrel this+0x40>*/ /*|0x1|*/ unsigned char Dir;
  /*<thisrel this+0x41>*/ /*|0x1|*/ unsigned char Action;
  /*<thisrel this+0x42>*/ /*|0x2|*/ short MemoSize;
  /*<thisrel this+0x44>*/ /*|0x12|*/ unsigned char Photo[18];
  /*<thisrel this+0x56>*/ /*|0x3e8|*/ char Memo[1000];
};
// <size 0x440>

struct FHP_FRIEND_MEMO_SEND_OLD 
{
	struct PWMSG_HEAD h;
	short Number;
	unsigned long WindowGuid;
	char Name[10];
	char ToName[10];
	char Subject[32];
	unsigned char Dir;
	unsigned char Action;
	short MemoSize;
	unsigned char Photo[13];
	char Memo[1000];
};

struct FHP_FRIEND_MEMO_SEND_RESULT
{
	PBMSG_HEAD h;	//	
	short Number;	//	4
	char Name[10];	//	6
	unsigned char Result;	//	10
	unsigned long WindowGuid;	//	14
};


struct FHP_FRIEND_MEMO_LIST_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ unsigned short Number;
  /*<thisrel this+0x6>*/ /*|0xa|*/ char Name[10];

};
// <size 0x10>


struct FHP_FRIEND_MEMO_LIST
{
	PWMSG_HEAD h;	//	
	unsigned short Number;	//	4
	unsigned short MemoIndex;	//	6
	char SendName[10];	//	8
	char RecvName[10];	//	12
	char Date[30];	//	1C
	char Subject[32];	//	3A
	unsigned char read;	//	5A
};

struct FHP_FRIEND_MEMO_RECV_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x6>*/ /*|0x2|*/ unsigned short MemoIndex;
  /*<thisrel this+0x8>*/ /*|0xa|*/ char Name[10];

};
// <size 0x12>


struct FHP_FRIEND_MEMO_RECV
{
	PWMSG_HEAD h;	//	
	short Number;	//	4
	char Name[10];	//	6
	unsigned short MemoIndex;	//	10
	short MemoSize;	//	12
	unsigned char Photo[18];	//	14
	unsigned char Dir;	//	26
	unsigned char Action;	//	27
	char Memo[1000];	//	28
};

struct FHP_FRIEND_MEMO_RECV_OLD
{
	PWMSG_HEAD h;
	short Number;
	char Name[10];
	unsigned short MemoIndex;
	short MemoSize;
	unsigned char Photo[13];
	unsigned char Dir;
	unsigned char Action;
	char Memo[1000];
};

struct FHP_FRIEND_MEMO_DEL_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x6>*/ /*|0x2|*/ unsigned short MemoIndex;
  /*<thisrel this+0x8>*/ /*|0xa|*/ char Name[10];

};
// <size 0x12>



struct FHP_FRIEND_MEMO_DEL_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char Result;	//	3
	unsigned short MemoIndex;	//	4
	short Number;	//	6
	char Name[10];	//	8
};


BOOL InitFriendServer(cWzExDBServer * sManager);
void CloseFriendServer();

DWORD __stdcall FriendProtocolCore(void *pArg);
DWORD __stdcall MailProtocolCore(void *pArg);


#pragma pack(1)
typedef struct _REQUEST_CMD
{
	int m_aIndex;
	BYTE m_btData[1024];
	_REQUEST_CMD()
	{
		ZeroMemory(this, sizeof(*this));
	}
}REQUEST_CMD, *LPREQUEST_CMD;
#pragma pack()

BOOL AddReqToFriendQueue(cWzExDBServer * sManager, int aIndex, LPBYTE lpReq);
LPREQUEST_CMD GetFriendReq();
void ClearFriendQueue();

BOOL AddReqToMailQueue(cWzExDBServer * sManager, int aIndex, LPBYTE lpReq);
LPREQUEST_CMD GetMailReq();
void ClearMailQueue();


void FriendListRequest(cWzExDBServer * sManager, int aIndex, FHP_FRIENDLIST_REQ* lpMsg);
void FriendStateClientRecv(cWzExDBServer * sManager, int aIndex, FHP_FRIEND_STATE_C* lpMsg);
void FriendAddRequest(cWzExDBServer * sManager, int aIndex, FHP_FRIEND_ADD_REQ* lpMsg);
void WaitFriendAddRequest(cWzExDBServer * sManager, int aIndex, FHP_WAITFRIEND_ADD_REQ* lpMsg);
void FriendDelRequest(cWzExDBServer * sManager, int aIndex, FHP_FRIEND_ADD_REQ* lpMsg);


void FriendMemoSend(cWzExDBServer * sManager, int aIndex, FHP_FRIEND_MEMO_SEND* lpMsg);
void FriendMemoListReq(cWzExDBServer * sManager, int aIndex, FHP_FRIEND_MEMO_LIST_REQ *lpMsg);
void FriendMemoReadReq(cWzExDBServer * sManager, int aIndex, FHP_FRIEND_MEMO_RECV_REQ* lpMsg);
void FriendMemoDelReq(cWzExDBServer * sManager, int aIndex, FHP_FRIEND_MEMO_DEL_REQ* lpMsg);

int ExDBGetIndexByCode(cWzExDBServer * sManager, int iServerCode);

BOOL UserGuidDBCreate(cWzExDBServer * sManager, char *szName);
BOOL GetDBGuidAndMemoTotal(cWzExDBServer * sManager, char *szName, int& guid, int& memo_total);
BOOL GetDBFriendList(cWzExDBServer * sManager, char *szMaster);


BOOL FriendExists(char *szMaster, char *szFriend);
int GetFriendGuid(char *szMaster);
BYTE GetFriendState(char *szMaster);
int GetFriendServer(char *szMaster);
int GetFriendNumber(char *szMaster);


void FriendListSend(cWzExDBServer * sManager, int aIndex, char *szMaster);

BOOL GetDBWaitFriend(cWzExDBServer * sManager, char *szMaster, OUT char *szWaitFriend);
BOOL FriendWaitSend(cWzExDBServer * sManager, int aIndex, char *szMaster);

void SendState(cWzExDBServer * sManager, int aIndex, char *szMaster, int iNumber, char *szName, BYTE btState);
void SendStateToAllFriends(cWzExDBServer * sManager, int aIndex, char *szMaster);

BOOL AddFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend);
int AddDBWaitFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend);
int DelDBWaitFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend);


int WaitFriendAdd(cWzExDBServer * sManager, char *szMaster, char *szFriend);
BOOL AddDBFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend);
BOOL DelDBFriend(cWzExDBServer * sManager, char *szMaster, char *szFriend);

void FriendClose(cWzExDBServer * sManager, int aIndex, LPBYTE lpMsg);



typedef struct _MEMO_HEADER
{
	unsigned short MemoIndex;
	char SendName[10];
	char RecvName[10];
	char Date[30];
	char Subject[32];
	unsigned char read;
}MEMO_HEADER, *LPMEMO_HEADER;


void MemoHeaderSend(cWzExDBServer * sManager, int aIndex, WORD wNumber, LPMEMO_HEADER lpMemoHead );
void MemoListSend(cWzExDBServer * sManager, int aIndex, WORD wNumber, char *szName);


typedef struct _MEMO_SEND_HEADER
{
	char Name[10];
	char ToName[10];
	char Subject[32];
	unsigned char Dir;
	unsigned char Action;
}MEMO_SEND_HEADER, *LPMEMO_SEND_HEADER;

int DBWriteMail(cWzExDBServer * sManager, LPMEMO_SEND_HEADER lpMemoSendHdr, LPBYTE Photo, BYTE btPhotoSize, char *sMemo, int memo_size);


typedef struct _MEMO_READ
{
	unsigned char Dir;
	unsigned char Action;
	char Memo[1000];
}MEMO_READ, *LPMEMO_READ;

BOOL DBReadMail(cWzExDBServer * sManager, int iMemoId, int iGuid, MEMO_READ & lpMemoRead, DWORD & lpdwMemoSize, LPBYTE lpPhoto, BYTE & lpPhotoSize);

BOOL DBDelMail(cWzExDBServer * sManager, char *szName, int iMemoId);