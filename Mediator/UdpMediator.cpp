#include "UdpMediator.h"

#include"../Net/UdpNet.h"
UdpMediator::UdpMediator(void)
{
	//new 网络对象
	m_pNet = new UdpNet(this);
}

UdpMediator::~UdpMediator(void)
{
	if( m_pNet )
	{
		delete m_pNet;
		m_pNet = NULL;
	}
}

//初始化网络
bool UdpMediator::OpenNet()
{
	if( !m_pNet->InitNet() )
		return false;
	return true;
}
//关闭网络
void UdpMediator::CloseNet()
{
	m_pNet->UnInitNet();
}
//发送数据
// 参考sendto  ip  port  -> sockaddr , buf , nlen
bool UdpMediator::SendData( unsigned long lSendIP , char* buf , int nlen )
{
	if( !m_pNet->SendData( lSendIP , buf , nlen ) )
		return false;
	return true;
}
#include <iostream>
//处理接收数据 // 来自谁的什么东西
void UdpMediator::DealData(unsigned long lSendIP , char* buf , int nlen )
{
	//处理接收 -- 回显 
    std::cout<<"ip " << INet::GetIPString( lSendIP) <<" :" << buf << std::endl;
    //Q_EMIT SIG_ReadyData( lSendIP , buf , nlen );

	SendData(lSendIP , buf , nlen );
//	delete[] buf;
}
