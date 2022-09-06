#pragma once

//网络实现中都会使用到
#include <winsock2.h>
//#include <ws2tcpip.h> //vs 2019

//#pragma comment(lib, "ws2_32.lib")   vs下生效的语句
#include"string"
#include<set>

class INetMediator;
class INet
{
public:
	INet(void);
	virtual ~INet(void);
public:
	//初始化网络
	virtual bool InitNet() = 0;
	//关闭网络
	virtual void UnInitNet() = 0;
	//发送数据
	// 参考sendto  ip  port  -> sockaddr , buf , nlen
	virtual bool SendData( unsigned long lSendIP , char* buf , int nlen ) = 0;

    //ip 从  long类型 转换为 字符串
    static std::string GetIPString(unsigned long ip)
    {
        sockaddr_in addr;
        addr.sin_addr.S_un.S_addr = ip;

        return inet_ntoa( addr.sin_addr );
    }

    //获取自己主机的ip列表  使用set容器
    static std::set<long>  GetValidIpList()
    {
        std::set<long> setip;
        char buf[100]="";
        if( SOCKET_ERROR != gethostname(buf , 100) )
        {
            struct hostent *remoteHost;
            int i = 0;
            struct in_addr addr;
            remoteHost = gethostbyname(buf);

            if (remoteHost->h_addrtype == AF_INET) {
                while (remoteHost->h_addr_list[i] != 0) {
                    addr.S_un.S_addr = *(u_long *) remoteHost->h_addr_list[i++];
                    //添加 ip地址到列表
                    setip.insert( addr.S_un.S_addr );
                }
            }
        }
        return setip;
    }

protected:
	//接收数据
	virtual void RecvData() = 0;
	INetMediator * m_pMediator;
};



