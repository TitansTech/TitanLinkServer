
#include "ServerManager.h"

struct SDHP_POSTINFO
{
	struct PBMSG_HEAD h;
	unsigned short ServerCode;
	char Name[11];
	char Chat[60];
};

struct SDHP_ALLSVRMSGSENDINFO
{
	struct PBMSG_HEAD h;
	unsigned short ServerCode;
	char Chat[60];
};


void PSProtocolCore(cPostServer * sManager, int aIndex, BYTE Head, LPBYTE pRecv, int iSize);
void PSDataServerLogin(cPostServer * sManager, int aIndex, SDHP_SERVERINFO * lpMsg);
void PSDataInsert(cDataServer * sManager, int aIndex, SDHP_POSTINFO * lpMsg);
void GetPost(cPostServer * sManager, int aIndex, SDHP_POSTINFO * lpMsg);
void GetAllSvrMsg(cPostServer * sManager, int aIndex, SDHP_ALLSVRMSGSENDINFO * lpMsg);
void SendAnnouncementPost(cPostServer * sManager, char * adminName, char * chat);
void SendAdminPost(cPostServer * sManager, char * adminName, char * chat);