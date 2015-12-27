#include "stdafx.h"
#include "shared.h"
#include "DSProtocol.h"

extern HWND hWnd;

void UpdateInstanceWindowTitle() 
{
	char sWindowTitle[1024]={0};
	wsprintf(sWindowTitle,"TitanLinkServer Running - %s", DATASERVER_CURRENT_VERSION);
	SetWindowText(hWnd,sWindowTitle);
}