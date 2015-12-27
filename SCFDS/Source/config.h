#ifndef DATASERVER_CONFIG_H
#define DATASERVER_CONFIG_H

#include <vector>
#include <string>

using namespace std;

typedef struct CONFIGSTRUCT 
{
	BYTE VipAutoRenew;
	int VipValue;
	int VipDays;
	BYTE IsS6E2;
	BYTE SavePostLogs;
	BYTE UseTTCS;
} *PCONFIGSTRUCT;

extern CONFIGSTRUCT g_Config;

void ConfigReadIniFile(const char *sFileName);

#endif //DATASERVER_CONFIG_H