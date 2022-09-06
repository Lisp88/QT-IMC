#include"Packdef.h"
#include<process.h>
#include "TcpServer.h"
#include"../Mediator/INetMediator.h"
#include "iostream"
TcpServer::TcpServer(INetMediator * pMediator): m_sock(INVALID_SOCKET), m_isStop(false)
{
    m_pMediator = pMediator;
}


TcpServer::~TcpServer(void)
{
    UnInitNet();
}
using namespace std;
//初始化网络
bool TcpServer::InitNet()
{
//    WORD wVersionRequested;
//    WSADATA wsaData;
//    int err;
//
//    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
//    wVersionRequested = MAKEWORD(2, 2);
//
//    err = WSAStartup(wVersionRequested, &wsaData);
//    if (err != 0) {
//        /* Tell the user that we could not find a usable */
//        /* Winsock DLL.                                  */
//        printf("WSAStartup failed with error: %d\n", err);
//        return 1;
//    }
//
//    /* Confirm that the WinSock DLL supports 2.2.*/
//    /* Note that if the DLL supports versions greater    */
//    /* than 2.2 in addition to 2.2, it will still return */
//    /* 2.2 in wVersion since that is the version we      */
//    /* requested.                                        */
//
//    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
//        /* Tell the user that we could not find a usable */
//        /* WinSock DLL.                                  */
//        printf("Could not find a usable version of Winsock.dll\n");
//        WSACleanup();
//        return 1;
//    }
//    else
//        printf("The Winsock 2.2 dll was found okay\n");
////2.创建套接字
//    m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//    if (m_sock == INVALID_SOCKET) {
//        printf("socket create fail:%d\n", WSAGetLastError());
//        WSACleanup();
//        system("pause");
//        return 1;
//    }
////3.绑定
//    struct sockaddr_in addrServer;  //储存传输端口信息的结构体
//    addrServer.sin_family = AF_INET;//ipv4
//    addrServer.sin_port = htons(8080);//端口
//    addrServer.sin_addr.S_un.S_addr = INADDR_ANY;//绑定任意网卡
//    //套接字 结构体地址 结构体大小
//    if (SOCKET_ERROR == bind(m_sock, (const sockaddr*)&addrServer, sizeof(addrServer)))
//    {
//        printf("bind fail:%d\n", WSAGetLastError());
//        closesocket(m_sock);
//        WSACleanup();
//        system("pause");
//        return 1;
//    }
////4.监听
//    if (0 != listen(m_sock, /*SOMAXCONY正整数最大值*/10))//一次接收的容量
//    {
//        printf("listen fail:%d\n", WSAGetLastError());
//        closesocket(m_sock);
//        WSACleanup();
//        system("pause");
//        return 1;
//    }
////5.接收连接
//    sockaddr_in addrClient;
//    int nSize = sizeof(addrClient);
//    SOCKET sockWaiter = accept(m_sock, (sockaddr*)&addrClient, &nSize);//套接字 套接字结构体地址 结构体大小
//    //sockWaiter与客人互相沟通
//    cout << "ip:" << inet_ntoa(addrClient.sin_addr) << "connect" << endl;
////6.接收数据
//    char buf[100] = "";
//    while (1)
//    {
//        int nRes = recv(sockWaiter, buf, sizeof(buf), 0);
//        if (nRes > 0) {
//            cout << "ip:" << inet_ntoa(addrClient.sin_addr) << "say:" << buf << endl;
////7.发送
//            gets(buf);
//            send(sockWaiter, buf, sizeof(buf), 0);
////
//        }
//        else if (nRes == 0) {
//            cout << "connect close" << endl;
//        }
//        else if (nRes < 0) {
//            printf("recv fail :%d\n", WSAGetLastError());
//            break;
//        }
//    }
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
    m_sock = socket( AF_INET , SOCK_STREAM , IPPROTO_TCP );//ipv4 , dgram , udp
    if( m_sock == INVALID_SOCKET )
    {
        WSACleanup();
        return false;
    }

    //3.找地 -- 绑定地址 bind()
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_DEF_TCP_PORT);//端口 网络字节序 -- 大端存储 低字节存在高地址 字节序颠倒 htons s->short 2字节
    addr.sin_addr.S_un.S_addr = INADDR_ANY/* inet_addr("192.168.1.168")*/; //绑定任意网卡

    if( bind( m_sock , (const sockaddr* )&addr , sizeof(addr) ) == SOCKET_ERROR )
    {
        UnInitNet();
        return false;
    }
    if(listen(m_sock, 10) != 0)
        return false;
    //4.接收和发送同时 , 使用线程完成  CreateThread
    //收数据 创建线程完成 接收  CreateThread WinAPI  一旦使用 strcpy这种 C/C++ RunTime 运行时库 创建内存块
    // CreateThread 接收线程时 会使用 ExitThread 退出线程 不回收内存块 导致内存泄露

    //_beginthreadex  内部是使用 CreateThread 完成实现 ,但是 退出线程 执行 _endthreadex  会回收内存块
    //这两个创建线程的方法使用起来都是一模一样的
    // 安全级别结构, 栈大小 , 函数指针 , 参数列表 , 初始状态 , 线程id
    m_accept_thread = (HANDLE)_beginthreadex( NULL , 0 , &AcceptThread , this , 0, NULL );
    if(!m_accept_thread){
        std::cout<<"接收线程创建失败"<<std::endl;
        return false;
    }

    return true;
}

//静态成员函数
unsigned int __stdcall TcpServer::RecvThread( void * lpvoid )
{
    TcpServer * pthis = (TcpServer *)lpvoid;
    pthis->RecvData();//出于方便的目的
    return 0;
}

//接收线程监听服务器socket
unsigned int __stdcall TcpServer::AcceptThread( void * lpvoid ){
    auto* m_p_tcp_server = (TcpServer*)lpvoid;
    sockaddr_in addr_client;
    int addr_size = sizeof(addr_client);
    while(!m_p_tcp_server->m_isStop){
        SOCKET sock_client = accept(m_p_tcp_server->m_sock, (sockaddr*)&addr_client, &addr_size);

        if(sock_client != SOCKET_ERROR){
            std::cout<<"ip : "<<inet_ntoa(addr_client.sin_addr)<<" connnect "<<std::endl;
            unsigned int thread_id = 0;
            auto handle = (HANDLE)_beginthreadex(nullptr, 0, &TcpServer::RecvThread, m_p_tcp_server, 0, &thread_id);
            if(handle){
                m_p_tcp_server->m_map_thread_id_to_socket[thread_id] = sock_client;
            }
        }
    }
    return 0;
}

//接收数据
void TcpServer::RecvData()
{   //TODO
    //防止map未更新，接收线程就开始工作，过后改成挂起唤醒
    Sleep(100);
    //通过线程ID获取socket
    SOCKET sock_client = m_map_thread_id_to_socket[GetCurrentThreadId()];
    if(!sock_client || sock_client == INVALID_SOCKET) return;
    //获取包大小
    int pack_size = 0;
    while(!m_isStop){
        int res = recv(sock_client, (char*)&pack_size, sizeof(int), 0);
        char *pack_buff = new char[pack_size];
        int offset = 0;
        //接收数据
        while(pack_size){
            res = recv(sock_client, pack_buff, pack_size, 0);
            if(res < 0) break;
            offset += res;
            pack_size -= res;
        }
        //中介者处理
        m_pMediator->DealData(sock_client, pack_buff, offset);
    }
}

//关闭网络
void TcpServer::UnInitNet()
{
    m_isStop = true;
    for( auto ite = m_recv_thread.begin(); ite != m_recv_thread.end(); ++ite)
    {
        if(*ite){
            //等待线程退出 100ms  没有退出销毁
            if( WaitForSingleObject(*ite , 100 ) == WAIT_TIMEOUT )
            {
                TerminateThread( *ite , -1 );
            }
            CloseHandle(*ite);
            *ite = NULL;
        }
    }
    m_recv_thread.clear();
    //服务器套接字
    if( m_sock && m_sock != INVALID_SOCKET )
        closesocket( m_sock );
    //客户端套接字
    for(auto ite = m_map_thread_id_to_socket.begin(); ite != m_map_thread_id_to_socket.end(); ++ite){
        if(ite->second && ite->second != INVALID_SOCKET){
            closesocket(ite->second);
        }
    }
    WSACleanup();
}

//发送数据
bool TcpServer::SendData( unsigned long lSendIP , char* buf , int nlen )
{
    if(!buf || nlen <= 0) return false;
    //发包大小
    if(send(lSendIP, (char*)&nlen, sizeof (nlen), 0) <= 0){
        return false;
    }
    //发数据包
    if(send(lSendIP, buf, nlen, 0) <= 0){
        return false;
    }

    return true;
}




