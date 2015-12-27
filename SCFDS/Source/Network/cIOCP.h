

#include "TSynchro.h"
#include <winsock2.h>

#define HIDE_ERRORL10
#define MAX_IO_BUFFER_SIZE		14000
#define MAX_IO_THREAD_HANDLES	16

struct _PER_IO_CONTEXT
{ 
	WSAOVERLAPPED Overlapped; // 0
	WSABUF wsabuf; 
	unsigned char Buffer[MAX_IO_BUFFER_SIZE]; // 1C
	unsigned char BufferSecond[MAX_IO_BUFFER_SIZE]; // 178C
	int nSecondOfs; // 2EFC
	int nTotalBytes;	// 2F00
	int nSentBytes; // 2F04
	int IOOperation; // 2F08
	int nWaitIO; // 2F0C

}; 

struct _PER_SOCKET_CONTEXT
{
	SOCKET m_socket;	// 0
	int nIndex;	// 4
	_PER_IO_CONTEXT IOContext[2];	// 8
	int dwIOCount;	// 5E28
}; 

#define sOBJMAX		1000

struct sOBJECT
{
	int				m_Index;
	char			m_ServerIp[16];
	bool			Connected;
	SOCKET			m_Socket;
	struct			_PER_SOCKET_CONTEXT * PerSocketContext;
};

class cIOCP
{
public:
	cIOCP();
	void Init(int ListenPort, int MaxObjects);
	BOOL DataSend(int aIndex, unsigned char* lpMsg, DWORD dwSize);
	BOOL qDataSend(int aIndex, unsigned char* lpMsg, DWORD dwSize);
	void DataSendAll(unsigned char* lpMsg, DWORD dwSize);
	//
	virtual void vLogAdd(char* szLog,...){};
	virtual void vLogAddC(BYTE Color, char* szLog, ...){};
	virtual void vLogAddTD(char* szLog, ...){};	
	virtual void OnConnect(int aIndex){};
	virtual void OnConnected(int aIndex){};
	virtual void OnDisconnect(int aIndex){};
	virtual void ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize){};
	//
	bool inline	isConnected(int aIndex){if(sObj[aIndex].Connected == true)return true;else return false;};
	SOCKET inline GetSocket(int aIndex){return sObj[aIndex].m_Socket;};
	
	int MaxObj;
	sOBJECT sObj[sOBJMAX];	

private:
	BOOL Create(int ListenPort);
	void Destroy();
	int CreateListenSocket();
	static DWORD WINAPI ServerWorker(void * p);
	static DWORD WINAPI ServerWorkerThread(void * p);
	BOOL RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex);
	BOOL IoSendSecond(_PER_SOCKET_CONTEXT * lpPerSocketContext);
	BOOL IoMoreSend(_PER_SOCKET_CONTEXT * lpPerSocketContext);
	BOOL UpdateCompletionPort(SOCKET sd, int ClientIndex, BOOL bAddToList);
	void CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result);
	void CloseClient(int index);
	void ResponErrorCloseClient(int index);
	void MsgBox(char *szlog, ...);
	//
	int ServerAddSearch();
	bool ServerAdd(SOCKET aSocket, char * ip, int aIndex);
	bool ServerDel(int index);

//Vars:
	
	TSynchroCriticalSection Sync;
	HANDLE g_CompletionPort;
	DWORD g_dwThreadCount;
	HANDLE g_ThreadHandles[MAX_IO_THREAD_HANDLES];
	int g_ServerPort;
	HANDLE g_IocpThreadHandle;
	SOCKET g_Listen;
};
