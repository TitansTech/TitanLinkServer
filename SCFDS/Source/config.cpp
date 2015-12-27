#include "stdafx.h"
#include "config.h"
#include "WinUtil.h"

CONFIGSTRUCT g_Config;

void ConfigReadIniFile(const char *sFileName)
{
	g_Config.VipAutoRenew = GetPrivateProfileInt("VIP", "IsAutoRenew", 0, sFileName);	
	g_Config.VipValue	  = GetPrivateProfileInt("VIP", "VipValue", 100, sFileName);	
	g_Config.VipDays	  = GetPrivateProfileInt("VIP", "Days", 10, sFileName);

	g_Config.IsS6E2			= GetPrivateProfileInt("Common", "IsS6E2", 0, sFileName);
	g_Config.SavePostLogs	= GetPrivateProfileInt("Common", "IsSavePostLogs", 0, sFileName);
	g_Config.UseTTCS		= GetPrivateProfileInt("Common", "UseTitanCS", 0, sFileName);


#if (WL_PROTECT==1)
	VM_START_WITHLEVEL(20)
		int MyCheckVar1;
		CHECK_PROTECTION(MyCheckVar1, 0x25454718)  
		if (MyCheckVar1 != 0x25454718)				
		{	
			g_Config.IsS6E2 = 0;
		}
	VM_END
#endif
}
