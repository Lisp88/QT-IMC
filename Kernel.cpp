//
// Created by Shota on 2022/9/5.
//

#include "Kernel.h"
#include "random"

//Kernel::Kernel():p_server(), sql(){}
std::default_random_engine e;
Kernel *Kernel::p_this = nullptr;

#define NET_PACK_MAP(n)         net_pack_map[n-_DEF_PROTOCOL_BASE]

void Kernel::set_net_pack_map() {
    NET_PACK_MAP(_DEF_TCP_LOGIN_RQ) = &Kernel::deal_login_data;
    NET_PACK_MAP(_DEF_TCP_REGISTER_RQ) = &Kernel::deal_register_data;
    NET_PACK_MAP(_DEF_TCP_ADD_RQ) = &Kernel::deal_add_friend;
    NET_PACK_MAP(_DEF_TCP_ADD_RS) = &Kernel::deal_friend_rs;
    NET_PACK_MAP(_DEF_TCP_CHAT_RQ) = &Kernel::deal_chat_rq;
    NET_PACK_MAP(_DEF_TCP_OFFLINE_INFO) = &Kernel::deal_offline_rq;
    NET_PACK_MAP(_DEF_TCP_FILE_RQ) = &Kernel::deal_file_rq;
    NET_PACK_MAP(_DEF_TCP_FILE_RS) = &Kernel::deal_file_rs;
    NET_PACK_MAP(_DEF_TCP_FILE_BLOCK_RQ) = &Kernel::deal_file_block_rq;//不写回复了
}

Kernel::Kernel() {
    p_this = this;
    set_net_pack_map();
}

bool Kernel::start_server() {
    //数据库连接
    if (!sql.ConnectMySql("127.0.0.1", "root", "88888888", "imc")) {
        cout << "mysql connect fail" << endl;
        return false;
    } else {
        cout << "mysql connect success" << endl;
    }
    //开启网络
    mask = 1;
    p_server = new TcpServerMediator;
    if (!p_server->OpenNet())
        return false;
    return true;
}

void Kernel::close_server() {
    //数据库断开
    sql.DisConnect();
    //关闭网络
    p_server->CloseNet();
    if (p_server) {
        delete p_server;
        p_server = nullptr;
    }
}

//处理包，根据协议分发包
void Kernel::deal_data(long send_ip, char *buff, int len) {
    //识别协议头，调用对应协议的函数
    this->mask;
    int type = *(int *) buff;
    (this->*NET_PACK_MAP(type))(send_ip, buff, len);
    //delete[] buff;
}

//登录回复包
void Kernel::deal_login_data(long send_ip, char *buff, int len) {
    //拆包
    S_TCP_LOGIN_RQ *login_rq = (S_TCP_LOGIN_RQ *) buff;
    std::cout << login_rq->telephone << ": login_rq" << std::endl;
    S_TCP_LOGIN_RS login_rs;
    list<string> do_result;
    char sql_buff[1024] = "";
    sprintf(sql_buff, "select id, password from user where telephone = '%s';", login_rq->telephone);
    if (!sql.SelectMySql(sql_buff, 2, do_result)) {
        cout << "deal_login_data: >> query tel and password fail" << endl;
    }
    if (do_result.size()) {
        int id = atoi(do_result.front().c_str());
        do_result.pop_front();
        string password = do_result.front();
        do_result.pop_front();
        if (strcmp(password.c_str(), login_rq->password) == 0) {
            login_rs.result = login_success;
            login_rs.userid = id;
            //成功登录，获取好友列表
            p_server->SendData(send_ip, (char *) &login_rs, sizeof(login_rs));
            online_pack_map[id] = send_ip;
            get_friend_list(id);
            return;
        } else {
            login_rs.result = password_error;
        }
    } else {
        //查无此人
        login_rs.result = user_not_exist;
    }
    //返回包信息
    p_server->SendData(send_ip, (char *) &login_rs, sizeof(login_rs));
}

//注册回复包
void Kernel::deal_register_data(long send_ip, char *buff, int len) {
    std::cout << "register_rq" << std::endl;
    S_TCP_REGISTER_RS register_rs;
    S_TCP_REGISTER_RQ *register_rq = (S_TCP_REGISTER_RQ *) buff;
    //查询
    char sql_buff[1024]{};
    list<string> query_result;
    sprintf(sql_buff, "select telephone "
                      "from user "
                      "where telephone = '%s';", register_rq->telephone);
    if (!sql.SelectMySql(sql_buff, 1, query_result)) {
        std::cout << "select tel fail" << std::endl;
    }

    if (query_result.size()) {
        register_rs.result = user_is_exist;
    } else {
        query_result.clear();
        sprintf(sql_buff, "select name from user where name = '%s';", register_rq->name);
        if (!sql.SelectMySql(sql_buff, 1, query_result)) {
            std::cout << "select name fail" << std::endl;
        }
        if (query_result.size()) {
            register_rs.result = name_is_exist;
        } else {
            register_rs.result = register_success;
//            std::uniform_int_distribution<int> u(0, 35);
            srand((int) time(0));
            sprintf(sql_buff, "insert into user(telephone, password, name, icon, feeling) "
                              "values('%s', '%s', '%s', %d, '普通用户');",
                    register_rq->telephone, register_rq->password, register_rq->name, rand() % 35);
            if (!sql.UpdateMySql(sql_buff)) {
                std::cout << "update fail : " << sql_buff << std::endl;
            }
        }
    }
    p_server->SendData(send_ip, (char *) &register_rs, sizeof(register_rs));
    delete buff;
}

//获取用户信息
void Kernel::get_user_info(int id, S_FRIEND_INFO *p_info) {
    p_info->userid = id;
    //数据库查询
    list<string> query_result;
    char sql_buff[1024]{};
    sprintf(sql_buff, "select name, icon, feeling from user where id = %d;", id);

    if (!sql.SelectMySql(sql_buff, 3, query_result)) {
        cout << "get_friend_list >> select user info fail" << endl;
    }

    if (query_result.size() == 3) {
        strcpy_s(p_info->name, sizeof(p_info->name), query_result.front().c_str());
        query_result.pop_front();
        p_info->icon = stoi(query_result.front());
        query_result.pop_front();
        strcpy_s(p_info->feeling, sizeof(p_info->feeling), query_result.front().c_str());
        query_result.pop_front();
    }
    if (online_pack_map.count(id))
        p_info->state = 1;
    else
        p_info->state = 0;
}

//获取好友列表，包括自己
void Kernel::get_friend_list(int id) {
    S_FRIEND_INFO self_info;
    get_user_info(id, &self_info);
    //if(online_pack_map.count(id) == 0) return;
    //发送自己
    SOCKET login_sock = online_pack_map[id];
    p_server->SendData(login_sock, (char *) &self_info, sizeof(self_info));

    //查询好友表
    S_FRIEND_INFO friend_info;
    list<string> query_result;
    char sql_buff[1024]{};
    sprintf(sql_buff, "select id_b from friend where id_a = %d;", self_info.userid);
    if (!sql.SelectMySql(sql_buff, 1, query_result)) {
        cout << "get_friend_list >> select friend id fail" << endl;
    }
    //发送好友表给登陆人
    while (!query_result.empty()) {
        int friend_id = stoi(query_result.front());
        query_result.pop_front();
        get_user_info(friend_id, &friend_info);

        cout << "发送好友信息给申请人 好友名字:" << friend_info.name << "好友id" << friend_info.userid << endl;
        p_server->SendData(login_sock, (char *) &friend_info, sizeof(friend_info));

        //发送登陆人信息给在线好友
        if (online_pack_map.count(friend_id)) {
            cout << "发送自身信息给在线好友" << endl;
            p_server->SendData(online_pack_map[friend_id], (char *) &self_info, sizeof(self_info));
        }
    }

}

//处理添加请求
void Kernel::deal_add_friend(long send_ip, char *buff, int len) {
    //拆包
    S_ADD_FRIEND_RQ *add_friend_rq = (S_ADD_FRIEND_RQ *) buff;
    //add_friend_rq->name;
    //查询
    list<string> query_result;
    char sql_buff[1024]{};
    sprintf(sql_buff, "select id from user where name = '%s';", add_friend_rq->friend_name);
    if (!sql.SelectMySql(sql_buff, 1, query_result)) {
        cout << "deal add friend >> select friend name fail" << endl;
    }

    S_ADD_FRIEND_RS add_friend_rs;
    if (!query_result.empty()) {
        int id = stoi(query_result.front());
        query_result.pop_front();
        //在线，发送申请
        cout << add_friend_rq->name << " 向 " << add_friend_rq->friend_name << "发送好友申请" << endl;
        //online_pack_map[5] = send_ip;
        if (online_pack_map.count(id)) {
            cout << "查询 id: " << id << "在线" << endl;
            SOCKET friend_sock = online_pack_map[id];
            cout << "向 id :" << id << "发送好友请求" << endl;
            p_server->SendData(friend_sock, (char *) add_friend_rq, len);
            return;
        } else {
            //不在线返回结果
            add_friend_rs.result = user_offline;
            strcpy_s(add_friend_rs.name, sizeof(add_friend_rq->name), add_friend_rq->name);
            add_friend_rs.user_id = add_friend_rq->user_id;
            strcpy_s(add_friend_rs.friend_name, sizeof(add_friend_rq->friend_name), add_friend_rq->friend_name);
        }
    } else {
        //没查到返回结果
        add_friend_rs.result = no_this_user;
    }
    p_server->SendData(send_ip, (char *) &add_friend_rs, sizeof(add_friend_rs));
}

//处理添加回复
void Kernel::deal_friend_rs(long send_ip, char *buff, int len) {
    //拆包
    S_ADD_FRIEND_RS *add_friend_rs = (S_ADD_FRIEND_RS *) buff;

    //根据结果进行对数据库的更新
    if (add_friend_rs->result == add_success) {
        cout << "好友 id :" << add_friend_rs->friend_id << "名字 :" << add_friend_rs->friend_name << "接受请求" << endl;
        char sql_buff[1024]{};
        sprintf(sql_buff, "insert friend values (%d, %d)", add_friend_rs->user_id, add_friend_rs->friend_id);
        if (!sql.UpdateMySql(sql_buff)) {
            cout << "Update >> insert friend info fail";
        }
        memset(sql_buff, 0, 1024);
        sprintf(sql_buff, "insert friend values (%d, %d)", add_friend_rs->friend_id, add_friend_rs->user_id);
        if (!sql.UpdateMySql(sql_buff)) {
            cout << "Update >> insert friend info fail";
        }
        //更新好友列表（传到客户端，刷新好友列表）
        cout << "发送更新 id :" << add_friend_rs->user_id << "名字 :" << add_friend_rs->name << "好友列表请求" << endl;
        get_friend_list(add_friend_rs->user_id);
    }
    //转发
    if (online_pack_map.count(add_friend_rs->user_id)) {
        p_server->SendData(online_pack_map[add_friend_rs->user_id], buff, len);
    }
}

//处理聊天请求
void Kernel::deal_chat_rq(long send_ip, char *buff, int len) {
    //拆包
    S_CHAT_RQ* chat_rq = (S_CHAT_RQ*)buff;
    //在线 转发
    if(online_pack_map.count(chat_rq->friendid)){
        p_server->SendData(online_pack_map[chat_rq->friendid], buff, len);
    }else{
        //不在线，写回复
        S_CHAT_RS chat_rs;
        chat_rs.userid = chat_rq->userid;
        chat_rs.friendid = chat_rq->friendid;
        chat_rs.result = user_offline;
        p_server->SendData(send_ip, (char*)&chat_rs, sizeof(chat_rs));
    }
}

//转发离线请求
void Kernel::deal_offline_rq(long send_ip, char *buff, int len) {
    //拆包
    S_TCP_OFFLINE* offline_rs = (S_TCP_OFFLINE*)buff;
    //向所有在线好友转发
    list<string> query_result;
    char sql_buff[1024]{};
    sprintf(sql_buff, "select id_b from friend where id_a = %d", offline_rs->userid);
    if(!sql.SelectMySql(sql_buff, 1, query_result)){
        cout<<"select friend and send offline >> select fail"<<endl;
    }
    while(!query_result.empty()){
        int id = stoi(query_result.front());

        if(online_pack_map.count(id)){
            p_server->SendData(online_pack_map[id], buff, len);
        }
        query_result.pop_front();
    }
    //从map移除
    if(online_pack_map.count(offline_rs->userid)) {
        online_pack_map.erase(offline_rs->userid);
        cout << "user offline userid :" << offline_rs->userid << endl;
    }
}

//转发请求
void Kernel::send_rq_to_id(int id, char *buff, int len) {
    if(online_pack_map.count(id)){
        p_server->SendData(online_pack_map[id], buff, len);
    }
}

void Kernel::deal_file_rq(long send_ip, char *buff, int len) {
    S_FILE_INFO_RQ* file_info_rq = (S_FILE_INFO_RQ*)buff;
    cout<<"file rq"<<endl;
    send_rq_to_id(file_info_rq->friend_id, buff, len);
}

void Kernel::deal_file_rs(long send_ip, char *buff, int len) {
    S_FILE_INFO_RS* file_info_rs = (S_FILE_INFO_RS*)buff;
    cout<<"file rs"<<endl;
    send_rq_to_id(file_info_rs->user_id, buff, len);
}

void Kernel::deal_file_block_rq(long send_ip, char *buff, int len) {
    S_FILE_BLOCK_RQ* file_block_rq = (S_FILE_BLOCK_RQ*)buff;
    cout<<"file block rq"<<endl;
    //send_rq_to_id(1, buff, len);
    send_rq_to_id(file_block_rq->friend_id, buff, len);
}
