#include "UdpNet.h"

#include"Packdef.h"
#include<process.h>
#include"../Mediator/INetMediator.h"

UdpNet::UdpNet(INetMediator * pMediator): m_sock(INVALID_SOCKET), m_hThreadHandle(NULL), m_isStop(false)
{
	m_pMediator = pMediator;
}


UdpNet::~UdpNet(void)
{
	UnInitNet();
}

//初始化网络
bool UdpNet::InitNet()
{
	//1.小买卖 炸串 卖奶茶 做煎饼果子 ... -- 选择一个 -- 加载库 WSAStartup();
	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        return false;
    }

/* Confirm that the WinSock DLL supports 2.2.*/
/* Note that if the DLL supports versions greater    */
/* than 2.2 in addition to 2.2, it will still return */
/* 2.2 in wVersion since that is the version we      */
/* requested.                                        */

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        WSACleanup();
        return false;
    }

	//2.雇人 -- 创建套接字 socket()
	m_sock = socket( AF_INET , SOCK_DGRAM , IPPROTO_UDP );//ipv4 , dgram , udp
	if( m_sock == INVALID_SOCKET )
	{
        WSACleanup();
        return false;
	}

	//发送广播 -- 设置广播权限
	BOOL bval = TRUE;
	setsockopt( m_sock , SOL_SOCKET/*哪一层*/ , SO_BROADCAST/*什么属性*/ , 
		/*值和空间大小*/(char*)&bval , sizeof(bval) );

	//3.找地 -- 绑定地址 bind()
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_UDP_PORT);//端口 网络字节序 -- 大端存储 低字节存在高地址 字节序颠倒 htons s->short 2字节
	//端口 1234
	addr.sin_addr.S_un.S_addr = INADDR_ANY/* inet_addr("192.168.1.168")*/; //绑定任意网卡

	if( bind( m_sock , (const sockaddr* )&addr , sizeof(addr) ) == SOCKET_ERROR )
	{
		UnInitNet();
        return false;
	}

	//4.接收和发送同时 , 使用线程完成  CreateThread
	//收数据 创建线程完成 接收  CreateThread WinAPI  一旦使用 strcpy这种 C/C++ RunTime 运行时库 创建内存块
	// CreateThread 接收线程时 会使用 ExitThread 退出线程 不回收内存块 导致内存泄露

	//_beginthreadex  内部是使用 CreateThread 完成实现 ,但是 退出线程 执行 _endthreadex  会回收内存块
	//这两个创建线程的方法使用起来都是一模一样的 
	 // 安全级别结构, 栈大小 , 函数指针 , 参数列表 , 初始状态 , 线程id
	m_hThreadHandle = (HANDLE)_beginthreadex( NULL , 0 , &RecvThread , this , 0, NULL );

	return true;
}

//静态成员函数 
unsigned int __stdcall UdpNet::RecvThread( void * lpvoid )
{
    UdpNet * pthis = (UdpNet *)lpvoid;
	pthis->RecvData();//出于方便的目的
//	pthis->m_sock;  出于方便的目的
	return 0;
}

//接收数据
void UdpNet::RecvData()
{
	char buf[_DEF_BUFFER ] = "";
	sockaddr_in addrClient;
	int nSize = sizeof(addrClient);

	while( !m_isStop )
	{
		//谁去接收 什么东西 来自谁, 
		int nRes = recvfrom( m_sock , buf , sizeof( buf ) , 0 , (sockaddr*)&addrClient ,&nSize ); 
		char* packbuf = new char[nRes];
		memcpy( packbuf , buf, nRes);
		if( nRes >0 )
		{
			//要处理 -- 中介处理
			m_pMediator->DealData( addrClient.sin_addr.S_un.S_addr , packbuf ,nRes ); // 来自谁的什么东西
			//执行函数要注意 回收该空间 delete[] packbuf;
		}
	}
}

//关闭网络
void UdpNet::UnInitNet()
{
	m_isStop = true;
	if( m_hThreadHandle )
	{
		//等待线程退出 100ms  没有退出销毁
		if( WaitForSingleObject(m_hThreadHandle , 100 ) == WAIT_TIMEOUT )
		{
			TerminateThread( m_hThreadHandle , -1 );
		}
		CloseHandle(m_hThreadHandle);m_hThreadHandle = NULL;
	}
	if( m_sock && m_sock != INVALID_SOCKET )
		closesocket( m_sock );

    WSACleanup();
}
//发送数据
// 参考sendto  ip  port  -> sockaddr , buf , nlen
bool UdpNet::SendData( unsigned long lSendIP , char* buf , int nlen )
{
	//向lSendIP发送 
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( _DEF_UDP_PORT );
	addr.sin_addr.S_un.S_addr = lSendIP;

	if( sendto( m_sock , buf, nlen , 0 , (sockaddr*)&addr , sizeof(addr) ) <= 0 )  // 谁向哪里发送
		return false;

	return true;
} 

