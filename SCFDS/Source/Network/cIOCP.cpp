#include "stdafx.h"
#include "cIOCP.h"
#include "..\..\stdafx.h"
#include "..\AllowManager.h"

cIOCP::cIOCP()
{
	this->g_Listen = INVALID_SOCKET;
}

void cIOCP::Init(int ListenPort, int MaxObjects)
{
	if(MaxObjects > sOBJMAX)
		MaxObjects = sOBJMAX;

	for(int i=0;i<MaxObjects;i++)
	{
		this->sObj[i].Connected = false;
		this->sObj[i].m_Index = -1;
		this->sObj[i].m_Socket = INVALID_SOCKET;
		this->sObj[i].PerSocketContext = new _PER_SOCKET_CONTEXT;
		memset(this->sObj[i].m_ServerIp,0,sizeof(this->sObj[i].m_ServerIp)-1);
	}

	this->MaxObj = MaxObjects;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
	this->Create(ListenPort);

	Sleep(100);
}

BOOL cIOCP::Create(int ListenPort)
{
	unsigned long ThreadID;
	
	this->g_ServerPort = ListenPort;

	this->g_IocpThreadHandle = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)this->ServerWorker, this, 0, &ThreadID);

	if ( this->g_IocpThreadHandle == 0 )
	{
		this->vLogAdd("CreateThread() failed with error %d", GetLastError());
		return 0;
	}
	else
	{
		return 1;
	}	
}

void cIOCP::Destroy()
{
	return;
}


int cIOCP::CreateListenSocket()
{
	sockaddr_in InternetAddr;
	int nRet;

	this->g_Listen=WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if ( this->g_Listen == -1 )
	{
		this->vLogAdd("WSASocket() failed with error %d", WSAGetLastError() );
		return 0;
	}
	else
	{
		InternetAddr.sin_family=AF_INET;
		InternetAddr.sin_addr.S_un.S_addr=htonl(0);
		InternetAddr.sin_port=htons(this->g_ServerPort);
		nRet=bind(this->g_Listen, (sockaddr*)&InternetAddr, 16);
		
		if ( nRet == -1 )
		{
			this->vLogAddC(2,"bind() failed with error %d", WSAGetLastError());
			this->MsgBox("error-L3 : cant bind port: %d", this->g_ServerPort);
			exit(1);
			return 0 ;
		}
		else
		{
			nRet=listen(this->g_Listen, 5);
			if (nRet == -1)
			{
				this->vLogAddC(2,"listen() failed with error %d", WSAGetLastError());
				this->MsgBox("error-L3 : listen() failed with error %d", WSAGetLastError());
				exit(1);
				return 0;
			}
			else
			{				
				this->vLogAddC(4,"Server Listen OK. Port: %d",this->g_ServerPort);
				return 1;
			}
		}
	} 
}


DWORD cIOCP::ServerWorker(void * p)
{
	cIOCP *	lpThis		= (cIOCP *)p;

	SYSTEM_INFO SystemInfo;
	DWORD ThreadID=0;
	SOCKET Accept;
	int nRet;
	int ClientIndex;
	sockaddr_in cAddr;
	in_addr cInAddr;
	int cAddrlen;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	int RecvBytes;
	unsigned long Flags;

	cAddrlen=16;
	lpPerSocketContext=0;
	Flags=0;

	GetSystemInfo(&SystemInfo);

	lpThis->g_dwThreadCount = SystemInfo.dwNumberOfProcessors * 2;
	__try
	{

		lpThis->g_CompletionPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

		if ( lpThis->g_CompletionPort == NULL )
		{
			lpThis->vLogAdd("CreateIoCompletionPort failed with error: %d", GetLastError());
			__leave;
		}

		for ( DWORD n = 0; n<lpThis->g_dwThreadCount; n++ )
		{
			

			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerWorkerThread, lpThis, 0, &ThreadID);

			if ( hThread == 0 )
			{
				lpThis->vLogAdd("CreateThread() failed with error %d", GetLastError() );
				__leave;
			}

			lpThis->g_ThreadHandles[n] = hThread;

			CloseHandle(hThread);
		}

		if ( lpThis->CreateListenSocket() == 0 )
		{

		}
		else
		{
			while ( true )
			{
				Accept = WSAAccept(lpThis->g_Listen, (sockaddr*)&cAddr, &cAddrlen, NULL, 0 );

				if ( Accept == -1 )
				{
					lpThis->Sync.Lock();
					lpThis->vLogAdd("WSAAccept() failed with error %d", WSAGetLastError() );
					lpThis->Sync.Unlock();
					continue;
				}

				lpThis->Sync.Lock();
				memcpy(&cInAddr, &cAddr.sin_addr  , sizeof(cInAddr) );

				ClientIndex = lpThis->ServerAddSearch();

				if(AllowManager.IPCheck(inet_ntoa(cInAddr)) == false)
				{
					lpThis->vLogAddC(2,"[AllowList] Rejected IP %s",inet_ntoa(cInAddr));
					closesocket(Accept);
					lpThis->Sync.Unlock();
					continue;
				}

				if ( ClientIndex == -1 )
				{
					lpThis->vLogAdd("error-L2 : ClientIndex = -1");
					closesocket(Accept);
					lpThis->Sync.Unlock();
					continue;
				}

				if (lpThis->UpdateCompletionPort(Accept, ClientIndex, 1) == 0 )
				{
					lpThis->vLogAdd("error-L1 : %d %d CreateIoCompletionPort failed with error %d", Accept, ClientIndex, GetLastError() );
					closesocket(Accept);
					lpThis->Sync.Unlock();
					continue;
				}

				if (lpThis->ServerAdd(Accept, inet_ntoa(cInAddr), ClientIndex) == false )
				{
					lpThis->vLogAdd("error-L1 : %d %d ServerObjAdd() failed with error %d", Accept, ClientIndex, GetLastError() );
					lpThis->Sync.Unlock();
					closesocket(Accept);
					continue;
				}
				
				memset(&lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].Overlapped, 0, sizeof(WSAOVERLAPPED));
				memset(&lpThis->sObj[ClientIndex].PerSocketContext->IOContext[1].Overlapped, 0, sizeof(WSAOVERLAPPED));

				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].wsabuf.buf = (char*)&lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].Buffer;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].wsabuf.len = MAX_IO_BUFFER_SIZE;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].nTotalBytes = 0;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].nSentBytes = 0;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].nWaitIO = 0;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].nSecondOfs = 0;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].IOOperation = 0;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[1].wsabuf.buf = (char*)lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].Buffer;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[1].wsabuf.len = MAX_IO_BUFFER_SIZE;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[1].nTotalBytes = 0;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[1].nSentBytes = 0;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[1].nWaitIO = 0;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[1].nSecondOfs = 0;
				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[1].IOOperation = 1;
				lpThis->sObj[ClientIndex].PerSocketContext->m_socket = Accept;
				lpThis->sObj[ClientIndex].PerSocketContext->nIndex = ClientIndex;

				nRet = WSARecv(Accept, &lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].wsabuf , 1, (unsigned long*)&RecvBytes, &Flags, 
						&lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].Overlapped, NULL);

				if ( nRet == -1 )
				{
					if ( WSAGetLastError() != WSA_IO_PENDING )
					{
						lpThis->vLogAdd("error-L1 : WSARecv() failed with error %d", WSAGetLastError() );
						lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].nWaitIO = 4;
						lpThis->CloseClient(lpThis->sObj[ClientIndex].PerSocketContext, 0);
						lpThis->Sync.Unlock();
						continue;
					}
				}

				lpThis->sObj[ClientIndex].PerSocketContext->IOContext[0].nWaitIO  = 1;
				lpThis->sObj[ClientIndex].PerSocketContext->dwIOCount++;
			
				lpThis->OnConnected(ClientIndex);

				lpThis->Sync.Unlock();
			}
		}
	}
	__finally
	{
		
		if ( lpThis->g_CompletionPort != NULL )
		{
			for ( int i = 0 ; i < lpThis->g_dwThreadCount ; i++ )
			{
				PostQueuedCompletionStatus( lpThis->g_CompletionPort , 0, 0, 0);
			}
		}

		if ( lpThis->g_CompletionPort != NULL )
		{
			CloseHandle(lpThis->g_CompletionPort);
			lpThis->g_CompletionPort = NULL;
		}
		if ( lpThis->g_Listen != INVALID_SOCKET )
		{
			closesocket( lpThis->g_Listen);
			lpThis->g_Listen = INVALID_SOCKET;
		}
	}

	return 1;
		
}


DWORD cIOCP::ServerWorkerThread(void * p)
{
	cIOCP *	lpThis							= (cIOCP *)p;

	HANDLE CompletionPort;
	DWORD dwIoSize=0;
	unsigned long RecvBytes;
	unsigned long Flags;
	DWORD dwSendNumBytes;
	BOOL bSuccess;
	int nRet;
#ifdef _WIN64
	ULONG_PTR ClientIndex=0;
#else
	DWORD ClientIndex=0;
#endif
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	LPOVERLAPPED lpOverlapped;
	_PER_IO_CONTEXT * lpIOContext;
	
	

	CompletionPort = lpThis->g_CompletionPort;
	dwSendNumBytes=0;
	bSuccess=0;
	lpPerSocketContext=0;
	lpOverlapped=0;
	lpIOContext=0;
	
	while ( true )
	{
		bSuccess=GetQueuedCompletionStatus( CompletionPort, &dwIoSize, &ClientIndex, &lpOverlapped, -1); // WAIT_FOREVER

		if (bSuccess == 0)
		{
			if (lpOverlapped != 0)
			{
				int aError = GetLastError();
				//Win2k3 SP2 Fix
				//#define ERROR_SEM_TIMEOUT                121L
				//The semaphore timeout period has expired.
				if ( (aError != ERROR_SEM_TIMEOUT) && (aError != ERROR_NETNAME_DELETED) && (aError != ERROR_CONNECTION_ABORTED) && (aError != ERROR_OPERATION_ABORTED) )
				{
					lpThis->Sync.Lock();
					lpThis->vLogAdd("Error Thread : GetQueueCompletionStatus( %d )", GetLastError());
					lpThis->Sync.Unlock();
					return 0;
				}
				//LogAdd("[TRACE] Error Thread : GetQueueCompletionStatus( %d )", GetLastError());
			}
		}

		lpThis->Sync.Lock();

		lpPerSocketContext= lpThis->sObj[ClientIndex].PerSocketContext;
		lpPerSocketContext->dwIOCount --;
				
		if ( dwIoSize == 0 )
		{
			lpThis->vLogAdd("Connection Closed, dwIoSize == 0 (Index:%d)", lpPerSocketContext->nIndex);
			lpThis->CloseClient(lpPerSocketContext, 0);
			lpThis->Sync.Unlock();
			continue;
		}

		lpIOContext = (_PER_IO_CONTEXT *)lpOverlapped;

		if ( lpIOContext == 0 )
		{
			continue;
		}

		if ( lpIOContext->IOOperation == 1 )
		{
			lpIOContext->nSentBytes += dwIoSize;


			if ( lpIOContext->nSentBytes >= lpIOContext->nTotalBytes )
			{
				lpIOContext->nWaitIO = 0;
						
				if ( lpIOContext->nSecondOfs > 0)
				{
					lpThis->IoSendSecond(lpPerSocketContext);
				}
			}
			else
			{
				lpThis->IoMoreSend(lpPerSocketContext);
			}
			
		}
		else if ( lpIOContext->IOOperation == 0 )
		{
			RecvBytes = 0;
			lpIOContext->nSentBytes += dwIoSize;

			if ( lpThis->RecvDataParse(lpIOContext, lpPerSocketContext->nIndex ) == 0 )
			{
				lpThis->vLogAdd("error-L1 : Socket Header error %d, %d", WSAGetLastError(), lpPerSocketContext->nIndex);
				lpThis->CloseClient(lpPerSocketContext, 0);
				lpThis->Sync.Unlock();
				continue;
			}

			lpIOContext->nWaitIO = 0;
			Flags = 0;
			memset(&lpIOContext->Overlapped, 0, sizeof (WSAOVERLAPPED));
			lpIOContext->wsabuf.len = MAX_IO_BUFFER_SIZE - lpIOContext->nSentBytes;
			lpIOContext->wsabuf.buf = (char*)&lpIOContext->Buffer[lpIOContext->nSentBytes];
			lpIOContext->IOOperation = 0;

			nRet = WSARecv(lpPerSocketContext->m_socket, &lpIOContext->wsabuf, 1, &RecvBytes, &Flags,
						&lpIOContext->Overlapped, NULL);

			if ( nRet == -1 )
			{
				if ( WSAGetLastError() != WSA_IO_PENDING)
				{
					lpThis->vLogAdd("WSARecv() failed with error %d", WSAGetLastError() );
					lpThis->CloseClient(lpPerSocketContext, 0);
					lpThis->Sync.Unlock();
					continue;
				}
			}

			lpPerSocketContext->dwIOCount ++;
			lpIOContext->nWaitIO = 1;
		}
		lpThis->Sync.Unlock();
	}
	return 1;
}



BOOL cIOCP::RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex)	
{
	unsigned char* recvbuf;
	int lOfs;
	WORD size;
	WORD headcode;

	// Check If Recv Data has More thatn 3 BYTES
	if ( lpIOContext->nSentBytes < 3 )
	{
		return TRUE;
	}

	// Initialize Variables
	lOfs=0;
	size=0;
	recvbuf = lpIOContext->Buffer;
	
	// Start Loop
	while ( true )
	{
				// Select packets with
		// C1 or C2 as HEader
		if ( recvbuf[lOfs] == 0xC1 ||
			 recvbuf[lOfs] == 0xC3 )
		{
			size = recvbuf[lOfs+1];
			headcode = recvbuf[lOfs+2];
		}
		else if ( recvbuf[lOfs] == 0xC2 ||
			      recvbuf[lOfs] == 0xC4 )
		{
			size = recvbuf[lOfs+1] * 256;
			size |= recvbuf[lOfs+2];
			headcode = recvbuf[lOfs+3];
		}
		// If HEader Differs - Second Generation Protocols
		else
		{
			this->vLogAdd("error-L1 : header error %d",recvbuf[lOfs]);
			return false;
		}

		// Check Size is leess thant 0
		if ( size <= 0 )
		{
			this->vLogAdd("error-L1 : size %d",size);
			return false;
		}

		// Check if Size is On Range
		if ( size <= lpIOContext->nSentBytes )
		{
			this->ProtocolCore(uIndex, headcode, &recvbuf[lOfs], size);

			lOfs += size;
			lpIOContext->nSentBytes  -= size;

			if ( lpIOContext->nSentBytes <= 0 )
			{
				break;
			}
		}
		else if ( lOfs > 0 )
		{
			if ( lpIOContext->nSentBytes < 1 )
			{
				this->vLogAdd("error-L1 : recvbuflen 1 %s %d", __FILE__, __LINE__);
				break;
			}

			if ( lpIOContext->nSentBytes < MAX_IO_BUFFER_SIZE ) 
			{
				memcpy(recvbuf, &recvbuf[lOfs], lpIOContext->nSentBytes);
				this->vLogAdd("Message copy %d", lpIOContext->nSentBytes);
			}
			break;
		
		}
		else
		{
			break;
		}
		
	}

	return true;
}

void cIOCP::DataSendAll(unsigned char* lpMsg, DWORD dwSize)
{
	for(int i=0; i<this->MaxObj ;i++)
	{
		if(this->sObj[i].Connected == true)
		{
			this->DataSend(i,lpMsg,dwSize);
		}
	}
}

BOOL cIOCP::DataSend(int aIndex, unsigned char* lpMsg, DWORD dwSize)
{
	int i = 0;
	unsigned char changedProtocol = 0;
	int aLen = sizeof(lpMsg);
	unsigned long SendBytes;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	unsigned char * SendBuf;
	this->Sync.Lock();

	if ( ((aIndex < 0)? FALSE : (aIndex > this->MaxObj-1)? FALSE : TRUE )  == FALSE )
	{
		this->vLogAdd("error-L2 : Index(%d) %x %x %x ", dwSize, lpMsg[0], lpMsg[1], lpMsg[2]);
		this->Sync.Unlock();
		return false;
	}

	SendBuf = lpMsg;

	if ( this->sObj[aIndex].Connected == false )
	{
		this->Sync.Unlock();
		return FALSE;
	}

	lpPerSocketContext= this->sObj[aIndex].PerSocketContext;

	if ( dwSize > sizeof(lpPerSocketContext->IOContext[0].Buffer))
	{
		this->vLogAdd("Error : Max msg(%d) %s %d", dwSize, __FILE__, __LINE__);
		this->CloseClient(aIndex);
		this->Sync.Unlock();
		return false;
	}

	_PER_IO_CONTEXT  * lpIoCtxt;

	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if ( lpIoCtxt->nWaitIO > 0 )
	{
		if ( ( lpIoCtxt->nSecondOfs + dwSize ) > MAX_IO_BUFFER_SIZE-1 )
		{
			this->vLogAdd("(%d)error-L2 MAX BUFFER OVER %d %d %d", aIndex, lpIoCtxt->nTotalBytes, lpIoCtxt->nSecondOfs, dwSize);
			lpIoCtxt->nWaitIO = 0;
			this->CloseClient(aIndex);
			this->Sync.Unlock();
			return true;
		}

		memcpy( &lpIoCtxt->BufferSecond[lpIoCtxt->nSecondOfs], SendBuf, dwSize);
		lpIoCtxt->nSecondOfs += dwSize;
		this->Sync.Unlock();
		return true;
	}

	lpIoCtxt->nTotalBytes = 0;
	
	if ( lpIoCtxt->nSecondOfs > 0 )
	{
		memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
		lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
		lpIoCtxt->nSecondOfs = 0;
	}

	if ( (lpIoCtxt->nTotalBytes+dwSize) > MAX_IO_BUFFER_SIZE-1 )
	{
		this->vLogAdd("(%d)error-L2 MAX BUFFER OVER %d %d", aIndex, lpIoCtxt->nTotalBytes, dwSize);
		lpIoCtxt->nWaitIO = 0;
		this->CloseClient(aIndex);
		this->Sync.Unlock();
		return FALSE;
	}

	memcpy( &lpIoCtxt->Buffer[lpIoCtxt->nTotalBytes], SendBuf, dwSize);
	lpIoCtxt->nTotalBytes += dwSize;
	lpIoCtxt->wsabuf.buf = (char*)&lpIoCtxt->Buffer;
	lpIoCtxt->wsabuf.len = lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes = 0;
	lpIoCtxt->IOOperation = 1;
	

	if ( WSASend( this->sObj[aIndex].m_Socket, &lpIoCtxt->wsabuf , 1, &SendBytes, 0, &lpIoCtxt->Overlapped, NULL) == -1 )
	{

		if ( WSAGetLastError() != WSA_IO_PENDING )	
		{
			lpIoCtxt->nWaitIO = 0;
			this->CloseClient(aIndex);
			this->Sync.Unlock();
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount ++;
	}
	
	
	lpIoCtxt->nWaitIO = 1;
	this->Sync.Unlock();
	return true;
}



BOOL cIOCP::qDataSend(int aIndex, unsigned char* lpMsg, DWORD dwSize)
{
	unsigned long SendBytes;
	_PER_IO_CONTEXT  lpIoCtxt = {0};
	memset(&lpIoCtxt,0,sizeof(lpIoCtxt));

	this->Sync.Lock();


	if ( ((aIndex < 0)? FALSE : (aIndex > this->MaxObj-1)? FALSE : TRUE )  == FALSE )
	{
		this->vLogAdd("error-L2 : Index(%d) %x %x %x ", dwSize, lpMsg[0], lpMsg[1], lpMsg[2]);
		this->Sync.Unlock();
		return false;
	}

	if ( this->sObj[aIndex].Connected == false )
	{
		this->Sync.Unlock();
		return FALSE;
	}

	_PER_SOCKET_CONTEXT lpPerSocketContext;

	memcpy(lpIoCtxt.Buffer, lpMsg, dwSize);
	lpIoCtxt.wsabuf.buf = (char*)lpMsg;
	lpIoCtxt.wsabuf.len = dwSize;
	lpIoCtxt.IOOperation = 1;

	if ( WSASend(this->sObj[aIndex].m_Socket, &lpIoCtxt.wsabuf, 1, &SendBytes, 0, &lpIoCtxt.Overlapped, NULL) == -1 )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
		{
			this->vLogAdd("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), this->sObj[aIndex].m_ServerIp);
			this->CloseClient(aIndex);
			this->Sync.Unlock();
			return false;
		}
	}
	else
	{
	}

	this->Sync.Unlock();
	return true;
}


BOOL cIOCP::IoSendSecond(_PER_SOCKET_CONTEXT * lpPerSocketContext)
{
	unsigned long SendBytes;
	int aIndex;
	_PER_IO_CONTEXT * lpIoCtxt;

	this->Sync.Lock();
	aIndex = lpPerSocketContext->nIndex;
	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if ( lpIoCtxt->nWaitIO > 0 )
	{
		this->Sync.Unlock();
		return false;
	}

	lpIoCtxt->nTotalBytes = 0;
	if ( lpIoCtxt->nSecondOfs > 0 )
	{
		memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
		lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
		lpIoCtxt->nSecondOfs = 0;
	}
	else
	{
		this->Sync.Unlock();
		return false;
	}

	lpIoCtxt->wsabuf.buf = (char*)&lpIoCtxt->Buffer;
	lpIoCtxt->wsabuf.len = lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes = 0;
	lpIoCtxt->IOOperation = 1;

	if ( WSASend(this->sObj[aIndex].m_Socket, &lpIoCtxt->wsabuf, 1, &SendBytes, 0, &lpIoCtxt->Overlapped, NULL) == -1 )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
		{
			this->vLogAdd("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), this->sObj[aIndex].m_ServerIp);
			this->CloseClient(aIndex);
			this->Sync.Unlock();
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount ++;
	}
	
	lpIoCtxt->nWaitIO = 1;
	this->Sync.Unlock();
	
	return true;
}


BOOL cIOCP::IoMoreSend(_PER_SOCKET_CONTEXT * lpPerSocketContext)
{
	unsigned long SendBytes;
	int aIndex;
	_PER_IO_CONTEXT * lpIoCtxt;

	this->Sync.Lock();
	aIndex = lpPerSocketContext->nIndex;
	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if ( (lpIoCtxt->nTotalBytes - lpIoCtxt->nSentBytes) < 0 )
	{
		this->Sync.Unlock();
		return false;
	}

	lpIoCtxt->wsabuf.buf = (char*)&lpIoCtxt->Buffer[lpIoCtxt->nSentBytes];
	lpIoCtxt->wsabuf.len = lpIoCtxt->nTotalBytes - lpIoCtxt->nSentBytes;
	lpIoCtxt->IOOperation = 1;

	if ( WSASend(this->sObj[aIndex].m_Socket, &lpIoCtxt->wsabuf, 1, &SendBytes, 0, &lpIoCtxt->Overlapped, NULL) == -1 )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
		{
			this->vLogAdd("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), this->sObj[aIndex].m_ServerIp);
			this->CloseClient(aIndex);
			this->Sync.Unlock();
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount ++;
	}
	
	
	lpIoCtxt->nWaitIO = 1;
	this->Sync.Unlock();
	return true;
}


BOOL cIOCP::UpdateCompletionPort(SOCKET sd, int ClientIndex, BOOL bAddToList)
{
	_PER_SOCKET_CONTEXT * lpPerSocketContext = NULL;

	HANDLE cp = CreateIoCompletionPort((HANDLE) sd, this->g_CompletionPort, ClientIndex, 0);

	if ( cp == 0 )
	{
		this->vLogAdd("CreateIoCompletionPort: %d", GetLastError() );
		return FALSE;
	}

	this->sObj[ClientIndex].PerSocketContext->dwIOCount = 0;
	return TRUE;
}


void cIOCP::CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result)
{
	int index = -1;
	index = lpPerSocketContext->nIndex ;

	if ( index >= 0 && index < this->MaxObj )
	{
		if ( this->sObj[index].m_Socket != INVALID_SOCKET )
		{
			if (closesocket(this->sObj[index].m_Socket) == -1 )
			{
				if ( WSAGetLastError() != WSAENOTSOCK )
				{
					return;
				}
			}

			this->sObj[index].m_Socket = INVALID_SOCKET;
		}

		this->ServerDel(index);
	}
}



void cIOCP::CloseClient(int index)
{
	if ( index < 0 || index > this->MaxObj-1 )
	{
		this->vLogAdd("error-L1 : CloseClient index error");
		return;
	}

	if ( this->sObj[index].Connected == false )
	{
		this->vLogAdd("error-L1 : CloseClient connect error");
		return;
	}

	this->Sync.Lock();

	if ( this->sObj[index].m_Socket != INVALID_SOCKET )
	{
		closesocket(this->sObj[index].m_Socket );
		this->sObj[index].m_Socket = INVALID_SOCKET;
	}
	else
	{
		this->vLogAdd("error-L1 : CloseClient INVALID_SOCKET");
	}

	this->Sync.Unlock();
}

void cIOCP::ResponErrorCloseClient(int index)
{
	if ( index < 0 || index > this->MaxObj-1 )
	{
		this->vLogAdd("error-L1 : CloseClient index error");
		return;
	}

	if ( this->sObj[index].Connected == false )
	{
		this->vLogAdd("error-L1 : CloseClient connect error");
		return;
	}

	this->Sync.Lock();
	closesocket(this->sObj[index].m_Socket);
	this->sObj[index].m_Socket = INVALID_SOCKET;

	if ( this->sObj[index].m_Socket == INVALID_SOCKET )
	{
		this->vLogAdd("error-L1 : CloseClient INVALID_SOCKET");
	}

	this->ServerDel(index);
	this->Sync.Unlock();
}


int cIOCP::ServerAddSearch()
{
	for(int count=0; count < this->MaxObj; count++)
	{
		if ( this->sObj[count].Connected == false )
		{
			return count;
		}
	}
	return -1;
}

bool cIOCP::ServerAdd(SOCKET aSocket, char * ip, int aIndex)
{
	if ( this->sObj[aIndex].Connected == true)
	{
		return false;
	}

	this->sObj[aIndex].m_Index = aIndex;
	this->sObj[aIndex].m_Socket = aSocket;
	this->sObj[aIndex].Connected = true;
	strcpy(this->sObj[aIndex].m_ServerIp, ip);
	this->vLogAdd("connect : [%d][%s]", aIndex, ip);

	OnConnect(aIndex);

	return true;
}

bool cIOCP::ServerDel(int index)
{
	if ( index < 0 || index > this->MaxObj )
	{
		this->vLogAdd ("(%s)(%d) = index over error (%d)", __FILE__, __LINE__, index);
		return false;
	}

	if ( this->sObj[index].Connected == false )
	{
		return false;
	}

	memset(this->sObj[index].m_ServerIp,0,sizeof(this->sObj[index].m_ServerIp)-1);
	this->sObj[index].Connected = false;

	OnDisconnect(index);

	return true;
}


void cIOCP::MsgBox(char *szlog, ...)
{
	char szBuffer[512]="";
	va_list pArguments;
	va_start(pArguments, szlog);
	vsprintf(szBuffer, szlog, pArguments);
	va_end(pArguments);
	MessageBox(NULL, szBuffer, "error", MB_OK|MB_APPLMODAL);
}