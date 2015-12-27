// stdafx.h: archivo de inclusión de los archivos de inclusión estándar del sistema
// o archivos de inclusión específicos de un proyecto utilizados frecuentemente,
// pero rara vez modificados
//

#pragma once

//--------------------------
// WinLicense Protection
//--------------------------
// 0 : Dissabled
// 1 : Enabled
#define WL_PROTECT			0

#define MEM_PROTECT			0
#define MEM_PROTECT_DEBUG	0


#pragma warning(disable: 4996 4172 4244 4101 4018 4646)

#define MAX_TYPE_PLAYER 7

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Excluir material rara vez utilizado de encabezados de Windows
// Archivos de encabezado de Windows:
#include <windows.h>
#include <winsock2.h>
#pragma comment ( lib , "WS2_32.lib" )

// Archivos de encabezado en tiempo de ejecución de C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#include <stdio.h>
#include <SQL.h>
#include <sqlext.h>
#include <vector>
#include <map>
#include <time.h>
#include <process.h>
#include "Source/prodef.h"
#include "Source\WinUtil.h"
#include "Resource.h"



#if (WL_PROTECT==1)
#include "Extra\\WinLicenseSDK.h"
//#pragma comment (lib,"Extra\\WinlicenseSDK.lib")
#endif

// TODO: mencionar aquí los encabezados adicionales que el programa necesita
