// SCFDS.cpp: define el punto de entrada de la aplicación.
//

#include "stdafx.h"
#include "SCFDS.h"
#include <direct.h>
#include "Source\shared.h"
#include "Source\config.h"
#include "Source\Log.h"
#include "Source\Protect.h"
#include "Source\DSProtocol.h"
#include "Source\JSProtocol.h"
#include "Source\ExServerProtocol.h"
#include "Source\ServerManager.h"
#include "Source\AllowManager.h"
#include <iostream>
#include <stdlib.h>
HWND hWnd;
HWND hWnd_Post;
HDC hdc;int ypos=0;
int Lines=0;
PAINTSTRUCT ps;

#define MAX_LOADSTRING 100

// Variables globales:
HINSTANCE hInst;								// Instancia actual
TCHAR szTitle[MAX_LOADSTRING];					// Texto de la barra de título
TCHAR szWindowClass[MAX_LOADSTRING];			// nombre de clase de la ventana principal

// Declaraciones de funciones adelantadas incluidas en este módulo de código:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void DSMain();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
#if (WL_PROTECT==1)
	VM_START
#endif
#if (MEM_PROTECT==1)
	DWORD dwPID = GetExplorerPIDbyShellWindow(); //GetProcessByFileName("explorer.exe"); 
	DWORD dwPIDP = GetParentProcessId();

	if (dwPID != dwPIDP)
	{
		//MsgBox("Parent: %d", dwPIDP);
		char *pBaseAddr = (char*)GetModuleHandle(NULL);

		// Change memory protection
		VirtualProtect(pBaseAddr, replacementRange, // Assume x86 page size
				PAGE_READWRITE, &dwPIDP);

		ZeroMemory(pBaseAddr, replacementRange);
	}
	if (executedParentSearch == 0)
	{
		ReplaceBytes(imageBaseOfGS,replacementPart,replacementRange);
	}
	removeFunctionsArray[ 0] = (DWORD)_tWinMain;
	removeFunctionsArray[ 2] = (DWORD)DSMain;
	removeFunctionsArray[ 4] = (DWORD)MyRegisterClass;
		
	removeFunctionsArray[ 1] = *(DWORD*)(removeFunctionsArray[ 0]+1);
	removeFunctionsArray[ 1] += jmpLength;
	removeFunctionsArray[ 3] = *(DWORD*)(removeFunctionsArray[ 2]+1);
	removeFunctionsArray[ 3] += jmpLength;
	removeFunctionsArray[ 5] = *(DWORD*)(removeFunctionsArray[ 4]+1);
	removeFunctionsArray[ 5] += jmpLength;
#endif

#if (WL_PROTECT==1)
	VM_END
#endif

	//UNREFERENCED_PARAMETER(hPrevInstance);
	//UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: colocar código aquí.
	MSG msg;
	HACCEL hAccelTable;

	// Inicializar cadenas globales
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SCFDS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
#if (MEM_PROTECT==1)
	#if (MEM_PROTECT_DEBUG==1)
		ReplaceBytes(removeFunctionsArray[4],replacementPart,jmpLength);						//Call to MyRegisterClass
		DestroyFunction(removeFunctionsArray[4] + removeFunctionsArray[5],replacementPart);		//MyRegisterClass
	#else
		DestroyFunction(removeFunctionsArray[4],replacementPart);								//MyRegisterClass
	#endif
#endif

	// Realizar la inicialización de la aplicación:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
#if (WL_PROTECT==1)
	VM_START
#endif

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCFDS));

	DSMain();
#if (MEM_PROTECT==1)
	#if (MEM_PROTECT_DEBUG==1)
		ReplaceBytes(removeFunctionsArray[0],replacementPart,jmpLength);						//Call to _tWinMain
		ReplaceBytes(removeFunctionsArray[0] + removeFunctionsArray[1],replacementPart,250);	//_tWinMain

		ReplaceBytes(removeFunctionsArray[2],replacementPart,jmpLength);						//Call to DSMain
		DestroyFunction(removeFunctionsArray[2] + removeFunctionsArray[3],replacementPart);		//DSMain
	#else
		ReplaceBytes(removeFunctionsArray[0],replacementPart,250);								//_tWinMain
		DestroyFunction(removeFunctionsArray[2],replacementPart);								//DSMain
	#endif
#endif

#if (WL_PROTECT==1)
	VM_END
#endif

	// Bucle principal de mensajes:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


void DSMain()
{	
#if (WL_PROTECT==1)
	VM_START
#endif

	/* Server initialization */
	_mkdir("Logs");
	LogInit(TRUE);
	DWORD dwStartTickCount = GetTickCount();	
	LogAdd("Initialized log instance");
	LogDateChange();
	//SendMessage(hWnd, WM_TIMER, WM_LOG_PAINT, NULL);

	LogAddC(3,"Initializing Servers...");
	AllowManager.ReadAllowList(ALISTFILE);

	ConfigReadIniFile("./Config.ini");
	DataServer1.Initialize("DataServer1");
	DataServer2.Initialize("DataServer2");
	JoinServer.Initialize("JoinServer");
	ExtraServer.Initialize("ExtraServer");
	WzExDBServer.Initialize("WzExDB");
	PostServer.Initialize("PostServer");

	LogAddC(3,"Servers initialization in %d msec", GetTickCount() - dwStartTickCount);
	

	UpdateInstanceWindowTitle();
	#if (WL_PROTECT==1)
	VM_END

		int MyCheckVar;
		CODEREPLACE_START
			CHECK_PROTECTION(MyCheckVar, 0x12345678)
			if (MyCheckVar != 0x12345678)  		 
			   g_Config.IsS6E2 = 0;
		CODEREPLACE_END
	#endif
}




//
//  FUNCIÓN: MyRegisterClass()
//
//  PROPÓSITO: registrar la clase de ventana.
//
//  COMENTARIOS:
//
//    Esta función y su uso son sólo necesarios si desea que el código
//    sea compatible con sistemas Win32 anteriores a la función
//    'RegisterClassEx' que se agregó a Windows 95. Es importante llamar a esta función
//    para que la aplicación obtenga iconos pequeños bien formados
//    asociados a ella.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCFDS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SCFDS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


//
//   FUNCIÓN: InitInstance(HINSTANCE, int)
//
//   PROPÓSITO: guardar el identificador de instancia y crear la ventana principal
//
//   COMENTARIOS:
//
//        En esta función, se guarda el identificador de instancia en una variable común y
//        se crea y muestra la ventana principal del programa.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	hInst = hInstance; // Almacenar identificador de instancia en una variable global

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
	  return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetTimer(hWnd, WM_LOG_PAINT, 1500, NULL);
	int FourHours = (((1000*60) * 60) * 4);
	SetTimer(hWnd, WM_FOURHOURS, FourHours, NULL);
	SetTimer(hWnd, WM_LOG_DATE_CHANGE, 60000, NULL);

	////Post
	//if(PostServer.m_ServerPort != -1)
	//{
	//	hWnd_Post = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	//	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	//	if (!hWnd_Post)
	//	{
	//	  return FALSE;
	//	}
	//	SetTimer(hWnd_Post, WM_POSTLOG_PAINT, 1500, NULL);
	//	SetTimer(hWnd_Post, WM_POSTLOG_DATE_CHANGE, 60000, NULL);
	//	ShowWindow(hWnd_Post, 0);
	//	UpdateWindow(hWnd_Post);
	//	SetWindowText(hWnd_Post,"[SCFLinkServer] SCFPostServer Window");  
	//	SetMenu(hWnd_Post,NULL);
	//}

	return TRUE;
}


// Mesage handler for User box.
INT_PTR CALLBACK Users(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId,wmEvent;
	switch (message)
	{
		case WM_INITDIALOG:
			//LogAdd(5,"User::Dlg :: INITDIALOG (FUNCTION NOT IN USE ONLY NEXT RELEASE)");
			HWND hlst;
#ifdef _WIN64
        	SetClassLong(hDlg, GCLP_HICON, (long)LoadIcon(0, IDI_APPLICATION));
#else
        	SetClassLong(hDlg, GCL_HICON, (long)LoadIcon(0, IDI_APPLICATION));
#endif
       		hlst = GetDlgItem(hDlg, IDC_LIST1);

			for(int i = 0 ; i != MAX_LOGIN_USER ; i++)
				if(g_MuLoginUserData[i].iUserNumber != -1)
					SendMessage(hlst, LB_ADDSTRING, 0, (LPARAM) g_MuLoginUserData[i].m_AccoundID);
				return TRUE;
			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 

			switch(wmId)
			{
				case ID_LoadUser:
				{
					HWND hlst;
#ifdef _WIN64
        			SetClassLong(hDlg, GCLP_HICON, (long)LoadIcon(0, IDI_APPLICATION));
#else
        			SetClassLong(hDlg, GCL_HICON, (long)LoadIcon(0, IDI_APPLICATION));
#endif
        			hlst = GetDlgItem(hDlg, IDC_LIST1);
					SendMessage(hlst, LB_RESETCONTENT, 0, NULL);
					
					for(int i = 0 ; i < MAX_LOGIN_USER ; i++)
					{
						if(g_MuLoginUserData[i].iUserNumber != -1)
							SendMessage(hlst, LB_ADDSTRING, 0, (LPARAM) g_MuLoginUserData[i].m_AccoundID);
					}			
				}break;

				case ID_DISCONNECT:
					{  
						char text[11]={0};
						HWND hlst;
#ifdef _WIN64
        				SetClassLong(hDlg, GCLP_HICON, (long)LoadIcon(0, IDI_APPLICATION));
#else
        				SetClassLong(hDlg, GCL_HICON, (long)LoadIcon(0, IDI_APPLICATION));
#endif
        				hlst = GetDlgItem(hDlg, IDC_LIST1);

						int count = SendMessage(hlst, LB_GETCOUNT, 0, 0);
						int iSelected = -1;

						// go through the items and find the first selected one
						for (int i = 0; i < count; i++)
						{
						  // check if this item is selected or not..
							if (SendMessage(hlst, LB_GETSEL, i, 0) > 0)
							{
								// yes, we only want the first selected so break.
								iSelected = i;
								break;
							}
						}

						// get the text of the selected item
						if (iSelected != -1)
						{
							SendMessage(hlst, LB_GETTEXT, (WPARAM)iSelected , (LPARAM)text);
							JoinServer.KickUser(text);
							SendMessage(hlst, LB_DELETESTRING, (WPARAM)iSelected, NULL);
						}

					}break;
			}
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

//POST SERVER WINDOW:
INT_PTR CALLBACK PostWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{

	case WM_INITDIALOG:
	{
		SetDlgItemText(hDlg,IDC_EDIT1,PostServer.Admin_Name);
		return (INT_PTR)TRUE;
	}break;

	case WM_CHAR:
	{
		if(wParam == VK_RETURN)
		{
			goto SendMsg;
		}
	}break;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_POST_OK )
		{
SendMsg:
			char Name[11]={0};
			char Text[49]={0};
			GetDlgItemText(hDlg,IDC_EDIT1, Name,10);
			GetDlgItemText(hDlg,IDC_EDIT2, Text,48);
			if(Text[0] != 0 && Name[0] != 0)
				PostServer.SendAdmin(Name,Text);
			SetDlgItemText(hDlg,IDC_EDIT2,"");
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == ID_POST_ANNOUNCE )
		{
			char Name[11]={0};
			char Text[49]={0};
			GetDlgItemText(hDlg,IDC_EDIT1, Name,10);
			GetDlgItemText(hDlg,IDC_EDIT2, Text,48);
			if(Text[0] != 0 && Name[0] != 0)
				PostServer.SendAnnouncement(Name,Text);
			SetDlgItemText(hDlg,IDC_EDIT2,"");
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL )
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//
//  FUNCIÓN: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PROPÓSITO: procesar mensajes de la ventana principal.
//
//  WM_COMMAND	: procesar el menú de aplicación
//  WM_PAINT	: pintar la ventana principal
//  WM_DESTROY	: enviar un mensaje de finalización y volver
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;

	switch (message)
	{

//	case WM_LBUTTONUP:
//			goto ShowPostWin;
//	case WM_CHAR:
//	{
//		if(wParam == VK_RETURN)
//		{
//ShowPostWin:
//			if(hWnd == hWnd_Post)
//				DialogBox(hInst, MAKEINTRESOURCE(IDD_POSTBOX), hWnd, PostWindow);
//		}
//	}break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Analizar las selecciones de menú:
		switch (wmId)
		{
		case ID_OPTIONS_DISCONNECTUSER:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_UserList),hWnd,Users);
			break;
		case IDM_POST:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_POSTBOX), hWnd, PostWindow);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_STATICS:
			JoinServer.Statics();
			break;
		//case ID_POSTSERVER_ADMINMESSAGE:	
		//	ShowWindow(hWnd_Post, 1);
		//	UpdateWindow(hWnd_Post);
		//	break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
		{
			switch ( wParam )
			{
				case WM_LOG_PAINT:
				{
					RECT rect;
					HDC hdc = GetDC(hWnd);
					GetClientRect(hWnd, &rect);
					FillRect(hdc, &rect, (HBRUSH)GetStockObject(0));
					ReleaseDC(hWnd, hdc);

					if ( LogTextPaint != NULL )
					{
						LogTextPaint(hWnd);
					}							
				}break;
				//case WM_POSTLOG_PAINT:
				//{
				//	if(PostServer.m_ServerPort != -1)
				//	{
				//		RECT rect;
				//		hdc = GetDC(hWnd_Post);
				//		GetClientRect(hWnd_Post, &rect);
				//		FillRect(hdc, &rect, (HBRUSH)GetStockObject(0));
				//		ReleaseDC(hWnd_Post, hdc);
				//		LogTextPaint(hWnd_Post);
				//	}
				//}break;
				case WM_FOURHOURS:
				{
					if(DSCheckNumber() == 1)
						DataServer1.SendGensRankingUpdate();
					else if(DSCheckNumber() == 2)
						DataServer2.SendGensRankingUpdate();
				}break;
				case WM_LOG_DATE_CHANGE:
				{
					if(DSCheckNumber() == 1)
						DataServer1.SendClearCoins();
					else if(DSCheckNumber() == 2)
						DataServer2.SendClearCoins();

					LogDateChange();
				}break;
			}
		}break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		if(hWnd == hWnd_Post)
		{
			ShowWindow(hWnd,0);
			break;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Controlador de mensajes del cuadro Acerca de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
