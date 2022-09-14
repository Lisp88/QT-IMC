//
// Created by Shota on 2022/8/24.
//
#include "TcpClient.h"
#include"Packdef.h"
#include<process.h>
#include"../Mediator/INetMediator.h"
#include "iostream"

TcpClient::TcpClient(INetMediator *pMediator) : m_sock(INVALID_SOCKET), m_hThreadHandle(NULL), m_isStop(false) {
    m_pMediator = pMediator;
}


TcpClient::~TcpClient(void) {
    UnInitNet();
}

//初始化网络
bool TcpClient::InitNet() {
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

    //2.创建套接字 socket()
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//ipv4 , stream , tcp
    if (m_sock == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    //3.绑定地址 bind()
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_DEF_TCP_PORT);//端口 网络字节序 -- 大端存储 低字节存在高地址 字节序颠倒 htons s->short 2字节
    //端口 1234
    addr.sin_addr.S_un.S_addr = inet_addr(_HOST_IP);


    //链接
    if (connect(m_sock, (sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR) {
        UnInitNet();
        return false;
    }else{
        std::cout<<"connect sussess"<<std::endl;
    }

    //4.接收和发送同时 , 使用线程完成  CreateThread
    m_hThreadHandle = (HANDLE) _beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);

    return true;
}

//静态成员函数
unsigned int __stdcall TcpClient::RecvThread(void *lpvoid) {
    TcpClient *pthis = (TcpClient *) lpvoid;
    std::cout<<"revc thread start"<<std::endl;
    pthis->RecvData();//出于方便的目的
//	pthis->m_sock;  出于方便的目的
    return 0;
}

//接收数据，防止粘包 先发包大小，再发包内容
void TcpClient::RecvData() {
    int pack_size = 0;
    int res = 0;
    while (!m_isStop) {
        //谁去接收 什么东西 来自谁,
        res = recv(m_sock, (char *) &pack_size, sizeof(int), 0);
        if(res <= 0){
            break;//服务器断开
        }
        char *pack_buff = new char[pack_size];
        int offset = 0;
        while (pack_size) {
            res = recv(m_sock, pack_buff + offset, pack_size, 0);
            if(res <= 0){
                break;//服务器断开
            }
            offset += res;//偏移，若包大小大于数据长度，则会卡在缓冲区等待填满
            pack_size -= res;
        }
        //要处理 -- 中介处理
        m_pMediator->DealData(m_sock, pack_buff, offset); // 来自谁的什么东西
        //执行函数要注意 回收该空间 delete[] packbuf;
    }
}

//关闭网络
void TcpClient::UnInitNet() {
    m_isStop = true;
    if (m_hThreadHandle) {
        //等待线程退出 100ms  没有退出销毁
        if (WaitForSingleObject(m_hThreadHandle, 100) == WAIT_TIMEOUT) {
            TerminateThread(m_hThreadHandle, -1);
        }
        CloseHandle(m_hThreadHandle);
        m_hThreadHandle = NULL;
    }
    if (m_sock && m_sock != INVALID_SOCKET)
        closesocket(m_sock);

    WSACleanup();
}

//发送数据
// 参考sendto  ip  port  -> sockaddr , buf , nlen
bool TcpClient::SendData(unsigned long lSendIP, char *buf, int nlen) {
    //用m_sock向服务器发送数据
    if (!buf || nlen <= 0) return false;
    //先发包大小
    send(m_sock, (char *) &nlen, 4, 0);
    //再发数据
    if (send(m_sock, buf, nlen, 0) <= 0)
        return false;

    return true;
}

