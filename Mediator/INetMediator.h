#pragma once

//#include<QObject>

class INet;
class INetMediator
{
//    Q_OBJECT
//signals:
    //void SIG_ReadyData( long lSendIP , char* buf , int nlen );
public:
	INetMediator() = default;
	~INetMediator();

public:
	//初始化网络
	virtual bool OpenNet() = 0;
	//关闭网络
	virtual void CloseNet() = 0;
	//发送数据
	// 参考sendto  ip  port  -> sockaddr , buf , nlen
	virtual bool SendData( unsigned long lSendIP , char* buf , int nlen ) = 0;
	//处理接收数据
	virtual void DealData( unsigned long lSendIP , char* buf , int nlen ) = 0; // 来自谁的什么东西
protected:
	
	INet * m_pNet;
};

