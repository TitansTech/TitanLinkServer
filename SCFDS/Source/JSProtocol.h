
#include "ServerManager.h"
#include "Network/WzUdp.h"
#include "prodef.h"



#define MAXIDLETIMEMS		15000
#define MAX_LOGIN_USER		(45000)
#define MAX_LOGIN_SERVER	(MAX_MAP_GROUPS*MAX_MAP_SUBGROUPS+1)
#define MAX_MAP_GROUPS		20
#define MAX_MAP_SUBGROUPS	20

enum PROTOCOL_ENUM {
	PROTO_HANDSHAKE			= 0x01,
	PROTO_VIPSTATE			= 0x02,
};

void InitDB();
// Core
void JSProtocolCore(cJoinServer * sManager,int aIndex, BYTE Head, unsigned char *pRecv, int aLen);

// Packets


struct SDHP_IDPASS
{
	PBMSG_HEAD h;	// C1:01	[11::LOGIN-TEST]
	char Id[10];	// 3
	char Pass[10];	// D
	short Number;	// 18
	char IpAddress[17];	// 1A
};

/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet User Close by Id
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x05
 */
struct SDHP_USERCLOSE_ID
{
	PBMSG_HEAD h;
	char szId[10];
	char szName[10];
	unsigned short Level;
	unsigned char DbClass;

};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Id Pass Result
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x01
 */
struct SDHP_IDPASSRESULT
{
	PBMSG_HEAD h;
	unsigned char result;	// 3
	short Number;	// 4
	char Id[10];	//	6
	int UserNumber;
	int DBNumber;
	char JoominNumber[13];
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x02
 */
struct SDHP_JOINFAIL
{
	PBMSG_HEAD h;
	short Number;
	char Id[11];
	int UserNumber;
	int DBNumber;

};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x04
 */
struct SDHP_COMMAND_BLOCK
{
	PBMSG_HEAD h;
	short Number;
	char Id[11];
	int UserNumber;
	int DBNumber;
	unsigned char BlockCode;
};

struct SDHP_SETGOLDCHANNELTIME
{
	PBMSG_HEAD h;
	char AccountID[11];
	short aIndex;
	DWORD GoldChannelTime;
};

struct SDHP_BILLSEARCH
{
	PBMSG_HEAD h;	// C1:06
	char Id[10];	// 3
	short Number;	// E
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Bill Search Result
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x06
 */
struct SDHP_BILLSEARCH_RESULT
{
	PBMSG_HEAD h;
	char Id[10];
	short Number;
	unsigned char cCertifyType;
	unsigned char PayCode;
	char EndsDays[12];
	int EndTime;
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Bill Kill User
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x07, 0x09
 */
struct SDHP_BILLKILLUSER
{
	struct PBMSG_HEAD h;
	char Id[10];
	short Number;
};







/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x08
 */
struct SDHP_OTHERJOINMSG
{
	struct PBMSG_HEAD h;
	char AccountID[11];
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x20
 */
struct SDHP_EXITMSG
{
	PBMSG_HEAD h;
	unsigned char ExitCode[3];
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x30
 */
struct SDHP_EVENTSTART
{
	PBMSG_HEAD h;
	unsigned char Event;
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x40
 */
struct SDHP_EVENTCONTROL
{
	PBMSG_HEAD h;
	unsigned char Event;
	unsigned char Stat;
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x41
 */
struct SDHP_OPTIONCONTROL
{
	PBMSG_HEAD h;
	unsigned char Option;
};





/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x30
 */
struct SDHP_LOVEHEARTEVENT
{
	PBMSG_HEAD h;
	char Account[10];
	char Name[10];
};






/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x61
 */
struct SDHP_LOVEHEARTEVENT_RESULT
{
	PBMSG_HEAD h;
	unsigned char Result;
	char Name[10];
	int Number;
};






/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x31
 */
struct SDHP_LOVEHEARTCREATE
{
	PBMSG_HEAD h;
	unsigned char x;
	unsigned char y;
	unsigned char MapNumber;
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x62
 */
struct SDHP_NOTICE
{
	PBMSG_HEAD h;
	BYTE unk[5];
	char Notice[61];
};

struct SDHP_NOTICE_S
{
	PBMSG_HEAD h;
	char Notice[61];
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x63
 */
struct SDHP_USER_NOTICE
{
	PBMSG_HEAD h;
	char szId[10];
	char Notice[61];
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x7A
 */
struct PMSG_REQ_MAPSVRMOVE
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	unsigned short wCurMapSvrCode;
	unsigned short wDstMapSvrCode;
	unsigned short wMapNumber;
	unsigned char btX;
	unsigned char btY;
};



/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Other Join Message
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x7A
 */
struct PMSG_ANS_MAPSVRMOVE
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	unsigned short wCurMapSvrCode;
	unsigned short wDstMapSvrCode;
	unsigned short wMapNumber;
	unsigned char btX;
	unsigned char btY;
	int iResult;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
};






/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Map Server Auth Answer
 *	Direction : JoinServer -> GameServer
 *  Code : 0xC1
 *	HeadCode : 0x7A ?????????????????????????????? #error
 */
struct PMSG_ANS_MAPSVRAUTH
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	WORD wPrevMapSvrCode;
	WORD wMapNumber;
	BYTE btX;
	BYTE btY;
	int iResult;
	int iUserNumber;
	int iDBNumber;
	char cJoominNumber[13];
	unsigned char btBlockCode;
};




/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Join Fail
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x7B 
 */
struct PMSG_REQ_MAPSVRAUTH
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	unsigned short wDstMapSvrCode;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
};


/* * * * * * * * * * * * * * * * * * * * * 
 *	Packet Notify Max User
 *	Direction : GameServer -> JoinServer
 *  Code : 0xC1
 *	HeadCode : 0x7C
 */
struct PMSG_NOTIFY_MAXUSER
{
	PBMSG_HEAD h;
	int iSvrCode;	// 4
	int iMaxUserCount;	// 8
	int iCurUserCount;	//	C
};


struct PMSG_JG_MEMO_SEND
{
	PWMSG_HEAD h;
	char Name[10];	// 4
	char TargetName[10];	// E
	char Subject[32];	// 18
	short MemoSize;	// 38
	char Memo[1000];	// 3A
};


// Objects
#pragma pack(1)

struct tagUSER_DATA 
{
	PMSG_ANS_MAPSVRMOVE pMapServerMoveData;
	int iUserNumber;
};


struct PMSG_USERNOTICE_RESULT
{
	PBMSG_HEAD	h;
	int			ClientIndex;
	BYTE		Result;
};

struct PMSG_USERNOTICE
{
	PBMSG_HEAD	h;
	int			ClientIndex;
	char		Notice[60];
	char		AccountID[10];
};


struct PMSG_GAME_BLOCK
{
	PBMSG_HEAD	h;
	int		ClientIndex;
	char	AccountID[11];
	BYTE	ServerNum;
	char	CharName[11];
	BYTE	Type;
};

extern int g_MuLoginServerDataCount;

#pragma pack()


struct SDHP_USERCONNECTION
{
	PBMSG_HEAD h;
	char Id[11];
};

// Protocol
void CSSendServerInfo(int SvrCode, BYTE State);
void CSSendServerInfo2(int Port, BYTE State);
void JGKillUserSend(cJoinServer * sManager, int aIndex, LPTSTR szAccountID);
void UserClose(cJoinServer * sManager, char * AccountID);
void GetGoldChannelInfo(cJoinServer * sManager, int aIndex, SDHP_SETGOLDCHANNELTIME * aRecv);
void GJAlreadyConnect(cJoinServer * sManager, int aIndex, SDHP_USERCLOSE_ID * aRecv );
void JoinServerLogin(cJoinServer * sManager, int aIndex, SDHP_SERVERINFO * lpMsg);
void JGPAccountRequest(cJoinServer * sManager, int aIndex, SDHP_IDPASS * aRecv);
void GJPAccountFail(cJoinServer * sManager, int aIndex, SDHP_JOINFAIL * aRecv);
void GJPAccountBlock(cJoinServer * sManager, int aIndex, SDHP_COMMAND_BLOCK * aRecv);
void GJPUserClose(cJoinServer * sManager, int aIndex, SDHP_USERCLOSE_ID * aRecv );
void JGOtherJoin(cJoinServer * sManager,int aIndex, LPTSTR szAccountID);
void GCJoinBillCheckSend(int aIndex, SDHP_BILLSEARCH * aRecv);
void LoveHeartEventRecv(cJoinServer * sManager,int aIndex, SDHP_LOVEHEARTEVENT * aRecv);
void LoveHeartCreateSend(cJoinServer * sManager,int aIndex, SDHP_LOVEHEARTCREATE * aRecv);
void GJReqMapSvrMove(cJoinServer * sManager,int aIndex, PMSG_REQ_MAPSVRMOVE * aRecv);
void GJReqMapSvrAuth(cJoinServer * sManager, int aIndex, PMSG_REQ_MAPSVRAUTH * aRecv);
void GJNotifyMaxUserCount(cJoinServer * sManager,int aIndex, PMSG_NOTIFY_MAXUSER * aRecv);
void NoticeRecv(cJoinServer * sManager,int aIndex, SDHP_NOTICE * aRecv);
void MngPro_GameBlock(cJoinServer * sManager,int aIndex,PMSG_GAME_BLOCK * lpMsg);
void MngPro_UserNoticeSend(cJoinServer * sManager,int aIndex,PMSG_USERNOTICE * lpMsg);
void SendGoldChannelTime(cJoinServer * sManager,char * szAccountID, DWORD GTime);
void StartCheckGoldTime(cJoinServer * sManager);
void ShowStatics(cJoinServer * sManager);
void KickUserFunc(cJoinServer * sManager, char * AccountID);

struct MAPSVRMOVE
{
	char szCharName[11];
	unsigned short wCurMapSvrCode;
	unsigned short wDstMapSvrCode;
	unsigned short wMapNumber;
	unsigned char btX;
	unsigned char btY;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
};

struct JOIN_SERVER_USER_DATA
{
	char m_AccoundID[11];
	WORD m_ServerCode;
	bool m_bRequestMapSvrMove;
	int iUserNumber;
	DWORD GoldTime;

	MAPSVRMOVE pMapMove;
};

extern JOIN_SERVER_USER_DATA g_MuLoginUserData[MAX_LOGIN_USER];

extern WzUdp g_ConnecServerUDP;
extern int g_MuLoginUserDataCount;