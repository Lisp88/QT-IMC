#pragma once
#include "inet.h"


class UdpNet : public INet
{
public:
    UdpNet(INetMediator * pMediator);
	~UdpNet(void);

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

