//
// Created by Shota on 2022/8/24.
//

#include "TcpClientMediator.h"
#include "../Net//Packdef.h"
#include"../Net/TcpClient.h"

TcpClientMediator::TcpClientMediator(void)
{
    //new 网络对象
    m_pNet = new TcpClient( this );
}

TcpClientMediator::~TcpClientMediator(void)
{
    if( m_pNet )
    {
        delete m_pNet;
        m_pNet = NULL;
    }
}

//初始化网络
bool TcpClientMediator::OpenNet()
{
    if( !m_pNet->InitNet() )
        return false;
    return true;
}
//关闭网络
void TcpClientMediator::CloseNet()
{
    m_pNet->UnInitNet();
}
//发送数据
// 参考sendto  ip  port  -> sockaddr , buf , nlen
bool TcpClientMediator::SendData( unsigned long lSendIP , char* buf , int nlen )
{
    if( !m_pNet->SendData( lSendIP , buf , nlen ) )
        return false;
    return true;
}
#include <iostream>
//处理接收数据 // 来自谁的什么东西
void TcpClientMediator::DealData(unsigned long lSendIP , char* buf , int nlen )
{
    //处理接收 -- 回显
    Q_EMIT signal_data_ready(buf , nlen );

}
