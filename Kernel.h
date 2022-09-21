//
// Created by Shota on 2022/9/5.
//
#include "Mysql/CMySql.h"
#include "Mediator/TcpServerMediator.h"
#include "iostream"
#include "map"
#include "Net/Packdef.h"
#ifndef IMC_KERNEL_H
#define IMC_KERNEL_H

class Kernel {
public:
    static Kernel* p_this;
    Kernel();
    bool start_server();
    void close_server();
    void set_net_pack_map();
    void get_friend_list(int id);
    void get_user_info(int id, S_FRIEND_INFO* p_info);
    //通过协议分发处理
    void deal_data(long send_ip, char* buff, int len);
    void deal_login_data(long send_ip, char* buff, int len);
    void deal_register_data(long send_ip, char* buff, int len);
    void deal_add_friend(long send_ip, char* buff, int len);
    void deal_friend_rs(long send_ip, char* buff, int len);
    void deal_chat_rq(long send_ip, char* buff, int len);
    void deal_offline_rq(long send_ip, char* buff, int len);
    void deal_file_rq(long send_ip, char* buff, int len);
    void deal_file_rs(long send_ip, char* buff, int len);
    void deal_file_block_rq(long send_ip, char* buff, int len);
    //转发请求
    void send_rq_to_id(int id, char* buff, int len);
private:
    CMySql sql;
    INetMediator* p_server{};
    typedef void (Kernel::*P_FUN)(long, char*, int);
    std::map<int, P_FUN> net_pack_map;//协议绑定成员函数
    int mask = 0;
    std::map<int, SOCKET> online_pack_map;//用户id绑定套接字
};


#endif //IMC_KERNEL_H
