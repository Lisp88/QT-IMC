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

    m_sock = socket( AF_INET , SOCK_STREAM , IPPROTO_TCP );//ipv4 , dgram , udp
    if( m_sock == INVALID_SOCKET )
    {
        WSACleanup();
        return false;
    }

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
        if(res < 0) {
            std::cout<<"client offline"<<std::endl;
            m_map_thread_id_to_socket.erase(GetCurrentThreadId());
            break;
        }
        char *pack_buff = new char[pack_size];
        int offset = 0;
        //接收数据
        while(pack_size){
            res = recv(sock_client, pack_buff, pack_size, 0);
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




