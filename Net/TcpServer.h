#ifndef TCPSERVER_H
#define TCPSERVER_H

#endif // TCPSERVER_H

#include "INet.h"
#include "list"
#include "map"
class TcpServer :public INet
{
public:
    TcpServer( INetMediator * pMediator);
    ~TcpServer(void);

public:
    //初始化网络
    bool InitNet();
    //关闭网络
    void UnInitNet();
    //发送数据
    // 参考sendto  ip  port  -> sockaddr , buf , nlen
    bool SendData( unsigned long lSendIP , char* buf , int nlen );
protected:
    //接收数据
    void RecvData();
    static unsigned int __stdcall RecvThread( void * lpvoid );
    static unsigned int __stdcall AcceptThread( void * lpvoid );
    bool m_isStop;
    HANDLE m_accept_thread;
    std::list<HANDLE> m_recv_thread;
    SOCKET m_sock;
    std::map<unsigned int, SOCKET> m_map_thread_id_to_socket;
};