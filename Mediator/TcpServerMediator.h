//
// Created by Shota on 2022/8/24.
//

#ifndef IMC_TCPSERVERMEDIATOR_H
#define IMC_TCPSERVERMEDIATOR_H

#endif //IMC_TCPSERVERMEDIATOR_H
#include "INetMediator.h"

class TcpServerMediator :
        public INetMediator
{
public:
    TcpServerMediator(void);
    ~TcpServerMediator(void);

public:
    //初始化网络
    bool OpenNet();
    //关闭网络
    void CloseNet();
    //发送数据
    // 参考sendto  ip  port  -> sockaddr , buf , nlen
    bool SendData(unsigned long lSendIP , char* buf , int nlen );
    //处理接收数据
    void DealData( unsigned long lSendIP, char* buf , int nlen ); // 来自谁的什么东西

protected:

};

