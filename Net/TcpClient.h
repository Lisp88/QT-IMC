//
// Created by Shota on 2022/8/24.
//
#include "INet.h"
#ifndef IMC_TCPCLIENT_H
#define IMC_TCPCLIENT_H
class TcpClient : public INet
{
public:
    TcpClient(INetMediator * pMediator);
    ~TcpClient(void);

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

    bool m_isStop;
    HANDLE m_hThreadHandle;
    SOCKET m_sock;
};
#endif //IMC_TCPCLIENT_H

