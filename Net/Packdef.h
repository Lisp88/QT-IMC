#pragma once
#include "memory.h"
#define _DEF_UDP_PORT  (1234)
#define _DEF_TCP_PORT   8080
#define _DEF_BUFFER	   (4096)
#define _DEF_SIZE		(60)
#define _DEF_CONTENT	(1024)
#define _SERVER_IP        "192.168.52.129"
#define _HOST_IP          "192.168.0.107"
//定义协议 : 上线请求 上线的回复  下线请求  聊天请求  聊天回复
//文件信息请求 (文件名 大小 文件id) 文件信息回复 文件块的请求  文件块的回复 

//协议头 从10000开始
#define _DEF_PROTOCOL_BASE		(10000)
#define _DEF_PROTOCOL_COUNT		(100)
//局域网UDP
//上线
#define _DEF_PROTOCOL_ONLINE_RQ		(_DEF_PROTOCOL_BASE+1)
#define _DEF_PROTOCOL_ONLINE_RS		(_DEF_PROTOCOL_BASE+2)
//下线
#define _DEF_PROTOCOL_OFFLINE		(_DEF_PROTOCOL_BASE+3)
//聊天信息
#define _DEF_PROTOCOL_DATAINFO_RQ		(_DEF_PROTOCOL_BASE+4)
#define _DEF_PROTOCOL_DATAINFO_RS		(_DEF_PROTOCOL_BASE+5)
//传文件
#define _DEF_PROTOCOL_FILEINFO_RQ		(_DEF_PROTOCOL_BASE+6)
#define _DEF_PROTOCOL_FILEINFO_RS		(_DEF_PROTOCOL_BASE+7)
//广域网 TCP
//注册 TCP
#define _DEF_TCP_REGISTER_RQ      (_DEF_PROTOCOL_BASE+11)
#define _DEF_TCP_REGISTER_RS      (_DEF_PROTOCOL_BASE+12)
//注册结果
#define user_is_exist       0
#define name_is_exist       1
#define register_success    2
//登录 TCP
#define _DEF_TCP_LOGIN_RQ      (_DEF_PROTOCOL_BASE+13)
#define _DEF_TCP_LOGIN_RS      (_DEF_PROTOCOL_BASE+14)
//登录结果
#define user_not_exist      0
#define password_error      1
#define login_success       2
//添加好友
#define _DEF_TCP_ADD_RQ     (_DEF_PROTOCOL_BASE+15)
#define _DEF_TCP_ADD_RS     (_DEF_PROTOCOL_BASE+16)
#define no_this_user        0
//好友结果
#define no_this_user        0
#define user_refuse         1
#define user_offline        2
#define add_success         3
//好友信息 TCP
#define _DEF_TCP_FRIEND_INFO   (_DEF_PROTOCOL_BASE+17)
//聊天
#define _DEF_TCP_CHAT_RQ        (_DEF_PROTOCOL_BASE+18)
#define _DEF_TCP_CHAT_RS        (_DEF_PROTOCOL_BASE+19)
//传文件
#define _DEF_TCP_FILE_RQ        _DEF_PROTOCOL_BASE+20
#define _DEF_TCP_FILE_RS        _DEF_PROTOCOL_BASE+21
//文件结果
//#define user_refuse         1 用上面的
//#define user_offline        2
#define file_accept         3
//离线
#define _DEF_TCP_OFFLINE_INFO       _DEF_PROTOCOL_BASE+22

//UDP
//协议结构 
//上线请求 包含内容? 协议头 , 名字 (ip 可以从接收获取 可以不用传输 ) 
//上线请求和回复时一致 可以复用  类型回复改变一下
struct STRU_ONLINE
{
	STRU_ONLINE():nType(_DEF_PROTOCOL_ONLINE_RQ){
		memset( szName , 0 , _DEF_SIZE );
	}
	int nType;
	char szName[_DEF_SIZE];
};

//下线
struct STRU_OFFLINE
{
	STRU_OFFLINE():nType(_DEF_PROTOCOL_OFFLINE){
	}
	int nType;
};

//聊天信息 : 协议头 序号 发送的内容 (ip不用传, 对方根据ip可以知道是谁发来的 )
struct STRU_DATAINFO_RQ
{
	STRU_DATAINFO_RQ():nType(_DEF_PROTOCOL_DATAINFO_RQ),nSeq(0){
		memset( szContent , 0 , _DEF_CONTENT );
	}
	int nType;
	int nSeq;
	char szContent[_DEF_CONTENT];
};

struct STRU_DATAINFO_RS
{
	STRU_DATAINFO_RS():nType(_DEF_PROTOCOL_DATAINFO_RS),nSeq(0){}
	int nType;
	int nSeq;
};

//TCP 自定义协议结构体
//注册
struct S_TCP_REGISTER_RQ{
    S_TCP_REGISTER_RQ():type(_DEF_TCP_REGISTER_RQ){}
    int type;
    char telephone[_DEF_SIZE]{};
    char password[_DEF_SIZE]{};
    char name[_DEF_SIZE]{};
};

struct S_TCP_REGISTER_RS{
    S_TCP_REGISTER_RS():type(_DEF_TCP_REGISTER_RS), result(register_success){}
    int type;
    int result;
};
//登录
struct S_TCP_LOGIN_RQ{
    S_TCP_LOGIN_RQ():type(_DEF_TCP_LOGIN_RQ){}
    int type;
    char telephone[_DEF_SIZE]{};
    char password[_DEF_SIZE]{};
};

struct S_TCP_LOGIN_RS{
    S_TCP_LOGIN_RS():type(_DEF_TCP_LOGIN_RS), result(login_success), userid(0){}
    int type;
    int result;
    int userid;//数据库中主键
};
//获取好友信息
struct S_FRIEND_INFO{
    S_FRIEND_INFO():type(_DEF_TCP_FRIEND_INFO), userid(0), icon(0), state(1){}
    int type;
    int userid;
    int icon;
    int state;
    char feeling[_DEF_SIZE]{};
    char name[_DEF_SIZE]{};
};

//添加好友
struct S_ADD_FRIEND_RQ
{
    S_ADD_FRIEND_RQ():type(_DEF_TCP_ADD_RQ), user_id(0) {}
    int type;
    int user_id;
    char name[_DEF_SIZE]{};
    char friend_name[_DEF_SIZE]{};
};
struct S_ADD_FRIEND_RS
{
    S_ADD_FRIEND_RS():type(_DEF_TCP_ADD_RQ), user_id(0) {}
    int type;
    int user_id;
    int friend_id;
    char name[_DEF_SIZE]{};
    char friend_name[_DEF_SIZE]{};
    int result;
};
//聊天
struct S_CHAT_RQ{
    S_CHAT_RQ():type(_DEF_TCP_CHAT_RQ), userid(0), friendid(0){}
    int type;
    int userid;
    int friendid;
    char content[_DEF_CONTENT];
};

struct S_CHAT_RS{
    S_CHAT_RS():type(_DEF_TCP_CHAT_RS), result(0){}
    int type;
    int result;
};

//离线
struct S_TCP_OFFLINE{
    S_TCP_OFFLINE():type(_DEF_TCP_OFFLINE_INFO), userid(0){}
    int type;
    int userid;
};

//数据库
