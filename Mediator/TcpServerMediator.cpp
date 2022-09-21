// Created by Shota on 2022/8/24.
//
#include "TcpServerMediator.h"
#include "../Net//TcpServer.h"
#include "../Kernel.h"
TcpServerMediator::TcpServerMediator(void)
{
    //new 网络对象
    m_pNet = new TcpServer( this );
}

TcpServerMediator::~TcpServerMediator(void)
{
    if( m_pNet )
    {
        delete m_pNet;
        m_pNet = nullptr;
    }
}

//初始化网络
bool TcpServerMediator::OpenNet()
{
    if( !m_pNet->InitNet() )
        return false;
    return true;
}
//关闭网络
void TcpServerMediator::CloseNet()
{
    m_pNet->UnInitNet();
}
//发送数据
// 参考sendto  ip  port  -> sockaddr , buf , nlen
bool TcpServerMediator::SendData( unsigned long lSendIP , char* buf , int nlen )
{
    if( !m_pNet->SendData( lSendIP , buf , nlen ) )
        return false;
    return true;
}
#include <iostream>
//处理接收数据 // 来自谁的什么东西
void TcpServerMediator::DealData( unsigned long lSendIP , char* buf , int nlen )
{
    Kernel::p_this->deal_data(lSendIP, buf, nlen);
}
