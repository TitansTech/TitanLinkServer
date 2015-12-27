#include "stdafx.h"
#include "AllowManager.h"
#include "ReadScript.h"
#include "Log.h"

cAllowManager AllowManager;

void cAllowManager::AllowListClean()
{
	for(int i=0;i<=MAXIPINALLOWLIST;i++)
	{
		memset(this->AllowListIP[i],0,sizeof(this->AllowListIP[i])-1);
	}
}

void cAllowManager::ReadAllowList(LPSTR filename)
{
	this->AllowListClean();

	int Token;
	this->ALSize=0;

	SMDFile = fopen(filename, "r");

	if ( SMDFile == NULL )
	{
		MsgBox("Error reading file %s", filename);
		exit(1);
	}

	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}

		//if ( Token == 1 )
		//{
		if(this->AddToAllowList(TokenString) == false)
		{
			MsgBox("error-L3 : Max IP count in AllowList reached!");
			exit(1);
		}
		//}
	}

	fclose(SMDFile);
	LogAddC(3, "[AllowList] Loaded Successfull!");
}

bool cAllowManager::IPCheck(LPSTR IP)
{
	for(int i=0;i<this->ALSize;i++)
		if(!stricmp(IP,this->AllowListIP[i]))
			return true;
	return false;
}

bool cAllowManager::AddToAllowList(LPSTR IP)
{
	if(this->ALSize >= MAXIPINALLOWLIST)
	{
		return false;
	}
	strcpy(this->AllowListIP[this->ALSize],IP);
	this->ALSize++;
	return true;
}