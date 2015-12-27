
#include "ServerManager.h"

using namespace std;
#define MAX_MEMBER_GUILD	80

struct SDHP_USERCLOSE
{
	PBMSG_HEAD h;	//	
	char CharName[10];	//	3
	char GuildName[8];	//	D
	unsigned char Type;	//	15
};


struct SDHP_GUILDCREATE
{
	PBMSG_HEAD h;
	char GuildName[9];	// 3
	char Master[11];	// C
	BYTE Mark[32];	// 17
	BYTE NumberH;	// 37
	BYTE NumberL;	// 38
	BYTE btGuildType;	// 39
};





struct SDHP_GUILDCREATE_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char Result;	//	3
	unsigned char Flag;	//	4
	unsigned long GuildNumber;	//	8
	unsigned char NumberH;	//	C
	unsigned char NumberL;	//	D
	char Master[11];	//	E
	char GuildName[9];	//	19
	unsigned char Mark[32];	//	22
	unsigned char btGuildType;	//	42
};



struct SDHP_GUILDDESTROY
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char GuildName[8];	// 5
	char Master[10];	// D
};


struct SDHP_GUILDDESTROY_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char Result;	//	3
	unsigned char Flag;	//	4
	unsigned char NumberH;	//	5
	unsigned char NumberL;	//	6
	char GuildName[9];	//	7
	char Master[11];	//	10
};


struct SDHP_GUILDMEMBERADD
{
	PBMSG_HEAD h;
	char GuildName[9];	// 3
	char MemberID[11];	// C
	BYTE NumberH;	// 17
	BYTE NumberL;	// 18
};


struct SDHP_GUILDMEMBERADD_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char Result;	//	3
	unsigned char Flag;	//	4
	unsigned char NumberH;	//	5
	unsigned char NumberL;	//	6
	char GuildName[9];	//	7
	char MemberID[11];	//	10
	short pServer;	//	1C
};



struct SDHP_GUILDMEMBERDEL
{
	PBMSG_HEAD h;	// C1:33
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char GuildName[8];	// 5
	char MemberID[10];	// D
};




struct SDHP_GUILDMEMBERDEL_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char Result;	//	3
	unsigned char Flag;	//	4
	unsigned char NumberH;	//	5
	unsigned char NumberL;	//	6
	char GuildName[9];	//	7
	char MemberID[11];	//	10
};




struct SDHP_GUILDUPDATE
{
	PBMSG_HEAD h;	//	
	char GuildName[9];	//	3
	char Master[11];	//	C
	unsigned char Mark[32];	//	17
	int Score;	//	28
	unsigned char Count;	//	3C
};



struct SDHP_GUILDMEMBER_INFO_REQUEST
{
	PBMSG_HEAD h;	// C1:35
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char MemberID[10];	// 5
};



struct SDHP_GUILDMEMBER_INFO
{
	PBMSG_HEAD h;	//	
	char GuildName[9];	//	3
	char MemberID[11];	//	C
	unsigned char btGuildStatus;	//	17
	unsigned char btGuildType;	//	18
	short pServer;	//	1A
};


struct SDHP_GUILDALL_COUNT
{
	PWMSG_HEAD h;
	int Number;	// 4
	char GuildName[9];	// 8
	char Master[11];	// 11
	unsigned char Mark[32];	// 1C
	int score;	// 3C
	BYTE btGuildType;	// 40
	int iGuildUnion;	// 44
	int iGuildRival;	// 48
	char szGuildRivalName[9];	// 4C
	unsigned char Count;	// 55
};

struct SDHP_GUILDALL
{
	char MemberID[11];	// 0
	BYTE btGuildStatus;	// B
	short pServer;	// C
};






struct SDHP_GUILDSCOREUPDATE
{
	PBMSG_HEAD h;	//	
	char GuildName[9];	//	3
	int Score;	//	C
};




struct SDHP_GUILDNOTICE
{
	PBMSG_HEAD h;	//	
	char GuildName[9];	//	3
	char szGuildNotice[60];	//	C
};




struct SDHP_GUILDCREATED
{
	PBMSG_HEAD h;	//	
	unsigned short Number;	//	4
	char GuildName[9];	//	6
	char Master[11];	//	F
	unsigned char Mark[32];	//	1A
	int score;	//	3C
};




struct SDHP_GUILDLISTSTATE
{
	PBMSG_HEAD h;	//	
	unsigned char State;	//	3
	int Count;	//	4
};



struct EXSDHP_GUILD_ASSIGN_STATUS_REQ
{
	PBMSG_HEAD h;
	WORD wUserIndex;	// 4
	BYTE btType;	// 6
	BYTE btGuildStatus;	// 7
	char szGuildName[9];	// 8
	char szTargetName[11];	// 11
};




struct EXSDHP_GUILD_ASSIGN_TYPE_REQ
{
	PBMSG_HEAD h;
	WORD wUserIndex;	// 4
	BYTE btGuildType;	// 6;
	char szGuildName[9];	// 7
};




struct EXSDHP_GUILD_ASSIGN_TYPE_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char btFlag;	//	3
	unsigned short wUserIndex;	//	4
	unsigned char btGuildType;	//	6
	unsigned char btResult;	//	7
	char szGuildName[9];	//	8
};




struct EXSDHP_GUILD_ASSIGN_STATUS_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char btFlag;	//	3
	unsigned short wUserIndex;	//	4
	unsigned char btType;	//	6
	unsigned char btResult;	//	7
	unsigned char btGuildStatus;	//	8
	char szGuildName[9];	//	9
	char szTargetName[11];	//	12
};



struct EXSDHP_SERVERGROUP_GUILD_CHATTING_RECV
{
	PBMSG_HEAD h;	//	
	int iGuildNum;	//	4
	char szCharacterName[10];	//	8
	char szChattingMsg[60];	//	12
};




struct EXSDHP_SERVERGROUP_UNION_CHATTING_RECV
{
	PBMSG_HEAD h;	//	
	int iUnionNum;	//	4
	char szCharacterName[10];	//	8
	char szChattingMsg[60];	//	12
};




struct EXSDHP_RELATIONSHIP_JOIN_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ unsigned short wRequestUserIndex;
  /*<thisrel this+0x6>*/ /*|0x2|*/ unsigned short wTargetUserIndex;
  /*<thisrel this+0x8>*/ /*|0x1|*/ unsigned char btRelationShipType;
  /*<thisrel this+0xc>*/ /*|0x4|*/ int iRequestGuildNum;
  /*<thisrel this+0x10>*/ /*|0x4|*/ int iTargetGuildNum;
};
// <size 0x14>





struct EXSDHP_RELATIONSHIP_JOIN_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char btFlag;	//	3
	unsigned short wRequestUserIndex;	//	4
	unsigned short wTargetUserIndex;	//	6
	unsigned char btResult;	//	8
	unsigned char btRelationShipType;	//	9
	int iRequestGuildNum;	//	C
	int iTargetGuildNum;	//	10
	char szRequestGuildName[9];	//	14
	char szTargetGuildName[9];	//	1D
};


struct EXSDHP_RELATIONSHIP_BREAKOFF_REQ 
{


  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ unsigned short wRequestUserIndex;
  /*<thisrel this+0x6>*/ /*|0x2|*/ unsigned short wTargetUserIndex;
  /*<thisrel this+0x8>*/ /*|0x1|*/ unsigned char btRelationShipType;
  /*<thisrel this+0xc>*/ /*|0x4|*/ int iRequestGuildNum;
  /*<thisrel this+0x10>*/ /*|0x4|*/ int iTargetGuildNum;
};
// <size 0x14>


struct EXSDHP_RELATIONSHIP_BREAKOFF_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char btFlag;	//	3
	unsigned short wRequestUserIndex;	//	4
	unsigned short wTargetUserIndex;	//	6
	unsigned char btResult;	//	8
	unsigned char btRelationShipType;	//	9
	int iRequestGuildNum;	//	C
	int iTargetGuildNum;	//	10
};




struct EXSDHP_NOTIFICATION_RELATIONSHIP
{
	PWMSG_HEAD h;	//	
	unsigned char btFlag;	//	4
	unsigned char btUpdateFlag;	//	5
	unsigned char btGuildListCount;	//	6
	int iGuildList[100];	//	8
};




struct EXSDHP_UNION_RELATIONSHIP_LIST
{
	PWMSG_HEAD h;	//	
	unsigned char btFlag;	//	4
	unsigned char btRelationShipType;	//	5
	unsigned char btRelationShipMemberCount;	//	6
	char szUnionMasterGuildName[9];	//	7
	int iUnionMasterGuildNumber;	//	10
	int iRelationShipMember[100];	//	14
};


struct EXSDHP_KICKOUT_UNIONMEMBER_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ unsigned short wRequestUserIndex;
  /*<thisrel this+0x6>*/ /*|0x1|*/ unsigned char btRelationShipType;
  /*<thisrel this+0x7>*/ /*|0x8|*/ char szUnionMasterGuildName[8];
  /*<thisrel this+0xf>*/ /*|0x8|*/ char szUnionMemberGuildName[8];

};
// <size 0x18>


struct EXSDHP_KICKOUT_UNIONMEMBER_RESULT
{
	PBMSG_HEAD2 h;	//	
	unsigned char btFlag;	//	4
	unsigned short wRequestUserIndex;	//	5
	unsigned char btRelationShipType;	//	8
	unsigned char btResult;	//	9
	char szUnionMasterGuildName[9];	//	A
	char szUnionMemberGuildName[9];	//	13
};

struct EXSDHP_UNION_LIST_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ unsigned short wRequestUserIndex;
  /*<thisrel this+0x8>*/ /*|0x4|*/ int iUnionMasterGuildNumber;
};
// <size 0xc>


struct EXSDHP_UNION_LIST 
{

  /*<thisrel this+0x0>*/ /*|0x1|*/ unsigned char btMemberNum;
  /*<thisrel this+0x1>*/ /*|0x20|*/ unsigned char Mark[32];
  /*<thisrel this+0x21>*/ /*|0x8|*/ char szGuildName[8];

};
// <size 0x29>

struct EXSDHP_UNION_LIST_COUNT 
{

  /*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x1|*/ unsigned char btCount;
  /*<thisrel this+0x5>*/ /*|0x1|*/ unsigned char btResult;
  /*<thisrel this+0x6>*/ /*|0x2|*/ unsigned short wRequestUserIndex;
  /*<thisrel this+0x8>*/ /*|0x4|*/ int iTimeStamp;
  /*<thisrel this+0xc>*/ /*|0x1|*/ unsigned char btRivalMemberNum;
  /*<thisrel this+0xd>*/ /*|0x1|*/ unsigned char btUnionMemberNum;
};
// <size 0x10>

struct FHP_FRIEND_CHATROOM_CREATE_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x6>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0x10>*/ /*|0xa|*/ char fName[10];
};
// <size 0x1a>


struct FHP_FRIEND_CHATROOM_CREATE_RESULT
{
	PBMSG_HEAD h;	//	
	unsigned char Result;	//	3
	short Number;	//	4
	char Name[10];	//	6
	char FriendName[10];	//	10
	char ServerIp[15];	//	1A
	unsigned short RoomNumber;	//	2A
	unsigned long Ticket;	//	2C
	unsigned char Type;	//	30
};


struct FCHP_CHATROOM_CREATE_REQ
{
	PBMSG_HEAD h;
	char Name[10];
	char FriendName[10];
	BYTE Type;
	short Number;
	short ServerId;
	short FriendNumber;
	short FriendServerId;
};


struct FCHP_CHATROOM_CREATE_RESULT
{
	PBMSG_HEAD h;
	BYTE Result;
	short RoomNumber;
	char Name[10];
	char FriendName[10];
	short Number;
	short ServerId;
	DWORD Ticket;
	DWORD Trash;
	BYTE Type;
};


struct FHP_FRIEND_INVITATION_REQ 
{

  /*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
  /*<thisrel this+0x4>*/ /*|0x2|*/ short Number;
  /*<thisrel this+0x6>*/ /*|0xa|*/ char Name[10];
  /*<thisrel this+0x10>*/ /*|0xa|*/ char FriendName[10];
  /*<thisrel this+0x1a>*/ /*|0x2|*/ unsigned short RoomNumber;
  /*<thisrel this+0x1c>*/ /*|0x4|*/ unsigned long WindowGuid;
};
// <size 0x20>


struct FHP_FRIEND_INVITATION_RET
{
	PBMSG_HEAD h;
	unsigned char Result;	// 3
	short Number;	// 4
	char Name[10];	// 6
	DWORD WindowGuid;	// 10
};


struct FCHP_CHATROOM_INVITATION_REQ
{
	PBMSG_HEAD h;
	short RoomNumber;
	char Name[10];
	short Number;
	short ServerId;
	BYTE Type;
};
#pragma pack(1)

struct strCmp
{
	bool operator()( const std::string s1, const std::string s2 ) const 
	{
		return strcmp( s1.data(), s2.data() ) < 0;
	}
};


typedef struct _GUILD_MEMBER
{
	char m_szMemberName[11];
	BYTE m_btStatus;
	BYTE m_btConnected;
	_GUILD_MEMBER()
	{
		ZeroMemory(m_szMemberName, sizeof(m_szMemberName));
		m_btStatus = 0;
		m_btConnected = -1;
	}
}GUILD_MEMBER, *LPGUILD_MEMBER;
typedef map<string, GUILD_MEMBER, strCmp> MAP_GUILD_MEMBER;


typedef struct tagGUILD_INFO_STRUCT
{
	int m_iNumber;
	char m_szGuildName[9];
	char m_szGuildMaster[11];
	BYTE m_btGuildType;
/*
	char m_szGuildMember[MAX_MEMBER_GUILD][10];
	BYTE m_btGuildStatus[MAX_MEMBER_GUILD];
	BYTE m_btConnected[MAX_MEMBER_GUILD];
*/
	MAP_GUILD_MEMBER m_mapGuildMember;
	int m_iRivalGuild;
	int m_iUnionGuild;
	BYTE m_Mark[32];
	int m_Score;
	char m_Notice[128];

	BOOL m_bGSHasData;
	tagGUILD_INFO_STRUCT()
	{
		m_iNumber = -1;
		memset(m_szGuildName, 0, sizeof(m_szGuildName));
		memset(m_szGuildMaster, 0, sizeof(m_szGuildMaster));
		m_btGuildType = -1;
		memset(m_Mark, 0, sizeof(m_Mark));
		memset(m_Notice,0,sizeof(m_Notice));
		m_iRivalGuild = 0;
		m_iUnionGuild = 0;
		m_Score = 0;
		m_mapGuildMember.clear();
		m_bGSHasData = FALSE;
	}

}GUILD_INFO_STRUCT, *LPGUILD_INFO_STRUCT;
typedef std::map<std::string, tagGUILD_INFO_STRUCT,strCmp> MAP_GUILD_INFO_STRUCT;



typedef struct tagUNION_DATA
{
	std::vector<int>	m_vecUnionMember;
//	std::vector<int>	m_vecRivalMember;
}UNION_DATA, *LPUNION_DATA;
typedef std::map<int, tagUNION_DATA> MAP_UNION_DATA;

#pragma pack()


void ExDataServerLogin(cWzExDBServer * sManager, int aIndex,  SDHP_SERVERINFO * lpMsg);
void ExDBProtocolCore(cWzExDBServer * sManager, int aIndex,  BYTE Head, LPBYTE aRecv, int iSize);
void GDCharCloseRecv(cWzExDBServer * sManager, int aIndex,  SDHP_USERCLOSE * aRecv);
void GDGuildCreateSend(cWzExDBServer * sManager, int aIndex,  SDHP_GUILDCREATE * aRecv);
void GDGuildDestroyRecv(cWzExDBServer * sManager, int aIndex,  SDHP_GUILDDESTROY * aRecv);
void GDGuildMemberAdd(cWzExDBServer * sManager, int aIndex,  SDHP_GUILDMEMBERADD * aRecv);
void GDGuildMemberDel(cWzExDBServer * sManager, int aIndex,  SDHP_GUILDMEMBERDEL * aRecv);
void DGGuildMemberInfoRequest(cWzExDBServer * sManager, int aIndex,  SDHP_GUILDMEMBER_INFO_REQUEST * aRecv);
void DGGuildScoreUpdate(cWzExDBServer * sManager, int aIndex,  SDHP_GUILDSCOREUPDATE * aRecv);
void GDGuildNoticeSave(cWzExDBServer * sManager, int aIndex,  SDHP_GUILDNOTICE * aRecv);
void GDGuildReqAssignStatus(cWzExDBServer * sManager, int aIndex,  EXSDHP_GUILD_ASSIGN_STATUS_REQ * aRecv);
void GDGuildReqAssignType(cWzExDBServer * sManager, int aIndex,  EXSDHP_GUILD_ASSIGN_TYPE_REQ * aRecv);
void GDGuildServerGroupChattingSend(cWzExDBServer * sManager, int aIndex, EXSDHP_SERVERGROUP_GUILD_CHATTING_RECV * aRecv);
void GDUnionServerGroupChattingSend(cWzExDBServer * sManager, int aIndex, EXSDHP_SERVERGROUP_UNION_CHATTING_RECV * aRecv);


void DGRelationShipAnsJoin(cWzExDBServer * sManager, int aIndex,  EXSDHP_RELATIONSHIP_JOIN_REQ * aRecv);
void DGRelationShipAnsBreakOff(cWzExDBServer * sManager, int aIndex,  EXSDHP_RELATIONSHIP_BREAKOFF_REQ * aRecv);
void DGUnionListRecv(cWzExDBServer * sManager, int aIndex,  EXSDHP_UNION_LIST_REQ * aRecv);
void DGRelationShipAnsKickOutUnionMember(cWzExDBServer * sManager, int aIndex,  EXSDHP_KICKOUT_UNIONMEMBER_REQ *aRecv);

void DGGuildMasterListSend(cWzExDBServer * sManager, char *szGuild);
void DGRelationShipListSend(cWzExDBServer * sManager, int aIndex,  int iGuild, int relation_type, BOOL snd_all);
void DGRelationShipNotificationSend(cWzExDBServer * sManager, int iGuild, int iUpdateFlag);

void FriendChatRoomCreateReq(cWzExDBServer * sManager, int aIndex,  FHP_FRIEND_CHATROOM_CREATE_REQ* lpMsg);
void FCHChatRoomCreateReq(cWzExDBServer * sManager, int aIndex,  char *szName, char *szFriendName, short Number, short ServerId, short FriendNumber, short FriendServerId);
void FriendChatRoomCreateAns(cWzExDBServer * sManager, int aIndex,  FCHP_CHATROOM_CREATE_RESULT* lpMsg);

void FriendChatRoomInvitationReq(cWzExDBServer * sManager, int aIndex,  FHP_FRIEND_INVITATION_REQ * lpMsg);
void FCHChatRoomInvitationReq(cWzExDBServer * sManager, int aIndex,  short RoomNumber,	char *szName, short Number,	short ServerId,	BYTE Type);

BOOL InitExDataServer(cWzExDBServer * sManager);
void InitDB();

void CloseExDataServer();

LPGUILD_INFO_STRUCT GetGuild(char *szGuild);
LPGUILD_INFO_STRUCT GetGuild(int iNumber);

LPGUILD_MEMBER GetGuildMember(char *szGuild, char *szName);

LPUNION_DATA GetUnionData(int iGuild);

int CreateDBGuild(cWzExDBServer * sManager, char* szGuild, char* szMaster, LPBYTE pMark);
int GetDBGuildNumber(cWzExDBServer * sManager, char *szGuild);

BOOL GuildExists(char *szGuild);
BOOL AddGuild(cWzExDBServer * sManager, char *szGuild, char *szMaster, int iNumber, char *lpNotice, int iScore, LPBYTE pMark, int iType);
BOOL DelGuild(char *szGuild);
BOOL MasterDBExists(cWzExDBServer * sManager, char *szMaster);
BOOL DelDBGuild(cWzExDBServer * sManager, char *szGuild);

BOOL GuildMemberExists(char *szGuild, char *szName);
BOOL AddGuildMember(cWzExDBServer * sManager, char *szGuild, char *szName, BYTE btStatus = 0, BYTE btConnected = -1);
BOOL DelAllDBGuildMember(cWzExDBServer * sManager, char *szGuild);
BOOL DBGuildMemberExists(cWzExDBServer * sManager, char *szName);
BOOL AddDBGuildMember(cWzExDBServer * sManager, char *szGuild, char *szName);
BOOL DelDBGuildMember(cWzExDBServer * sManager, char *szName);
BOOL DelGuildMember(char *szGuild, char *szName);

BOOL UpdateGuildMemberStatus(cWzExDBServer * sManager, char *szGuild, char *szName, BYTE btStatus);
BOOL UpdateGuildType(cWzExDBServer * sManager, char *szGuild, BYTE btType);

void SendGuildMemberInfo(cWzExDBServer * sManager, int aIndex,  char *szName);
BOOL GetGuildMemberInfo(cWzExDBServer * sManager, char *szName, OUT char *szGuild, OUT int& riLevel, OUT int& riStatus);


BOOL IsCSGuild(cWzExDBServer * sManager, char *szGuild);

BOOL UpdateScore(cWzExDBServer * sManager, char *szGuild, int iScore);
BOOL SaveNotice(cWzExDBServer * sManager, char *szGuild, char *szNotice);


BOOL CreateRelationShipData(int iGuild);
BOOL DestroyRelationShipData(int iGuild);

int RelationShipOperation(cWzExDBServer * sManager, int iReqGuild, int iTargGuild, int relation_type, int operation);

int RelationShipJoin(cWzExDBServer * sManager, LPGUILD_INFO_STRUCT lpReqGuild, LPGUILD_INFO_STRUCT lpTargGuild, int type);
int UnionJoin(cWzExDBServer * sManager, LPGUILD_INFO_STRUCT lpReqGuild, LPGUILD_INFO_STRUCT lpTargGuild);
int RivalJoin(cWzExDBServer * sManager, LPGUILD_INFO_STRUCT lpReqGuild, LPGUILD_INFO_STRUCT lpTargGuild);

int RelationShipBreak(cWzExDBServer * sManager, LPGUILD_INFO_STRUCT lpMasterGuild, LPGUILD_INFO_STRUCT lpKickGuild, int type);
int UnionBreak(cWzExDBServer * sManager, LPGUILD_INFO_STRUCT lpMasterGuild, LPGUILD_INFO_STRUCT lpKickGuild);
int RivalBreak(cWzExDBServer * sManager, LPGUILD_INFO_STRUCT lpMasterGuild, LPGUILD_INFO_STRUCT lpKickGuild);

BOOL AddUnion(LPGUILD_INFO_STRUCT lpReqGuild, LPGUILD_INFO_STRUCT lpTargGuild);
BOOL AddUnion(int iReqGuild, int iTargGuild);
BOOL KickUnion(LPGUILD_INFO_STRUCT lpMasterGuild, LPGUILD_INFO_STRUCT lpKickGuild);


BOOL UpdateDBUnion(cWzExDBServer * sManager, int iGuild, int iUnion);
BOOL UpdateDBRival(cWzExDBServer * sManager, int iGuild, int iRival);

void RelationShipBreakSend(int iGuild, int iType);
void SendListToAllRivals(cWzExDBServer * sManager, LPGUILD_INFO_STRUCT lpGuild);

int GetChatServer(cWzExDBServer * sManager);
void InitDB();