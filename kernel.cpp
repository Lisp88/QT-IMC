#include "kernel.h"
#include <QMessageBox>
#include <QDebug>
#include "Packdef.h"
#include "userchildwidget.h"
#include<QInputDialog>
#include<QSize>
#include "QFileDialog"
#define NET_PACK_MAP(n)         net_pack_map[n-_DEF_PROTOCOL_BASE]

void Kernel::set_net_pack_map() {
    //NET_PACK_MAP(_DEF_TCP_LOGIN_RQ) = &Kernel::deal_login_data;
    NET_PACK_MAP(_DEF_TCP_REGISTER_RS) = &Kernel::deal_register_rs;
    NET_PACK_MAP(_DEF_TCP_LOGIN_RS) = &Kernel::deal_login_rs;
    NET_PACK_MAP(_DEF_TCP_FRIEND_INFO) = &Kernel::deal_friend_rs;
    NET_PACK_MAP(_DEF_TCP_ADD_RS) = &Kernel::deal_add_rs;
    NET_PACK_MAP(_DEF_TCP_ADD_RQ) = &Kernel::deal_friend_rq;
    NET_PACK_MAP(_DEF_TCP_CHAT_RQ) = &Kernel::deal_chat_rq;
    NET_PACK_MAP(_DEF_TCP_CHAT_RS) = &Kernel::deal_chat_rs;
    NET_PACK_MAP(_DEF_TCP_OFFLINE_INFO) = &Kernel::deal_offline_rq;
    NET_PACK_MAP(_DEF_TCP_FILE_RQ) = &Kernel::deal_file_rq;
    NET_PACK_MAP(_DEF_TCP_FILE_RS) = &Kernel::deal_file_rs;
    NET_PACK_MAP(_DEF_TCP_FILE_BLOCK_RQ) = &Kernel::deal_file_block_rq;//不写回复了
}

Kernel::Kernel(QObject *parent) : QObject(parent), user_id(0)
{
    //为map赋值
    set_net_pack_map();
    //窗体
    p_login_windows = new DialogLogin;
    p_main_windows = new DialogMain;
    connect(p_main_windows, SIGNAL(signal_add_friend()), this, SLOT(slot_add_friend()));//绑定添加好友
    connect(p_main_windows, SIGNAL(signal_close()), this, SLOT(slot_offline()));

    //信号槽绑定
    connect(p_login_windows, SIGNAL(signal_register(QString,QString,QString)), this, SLOT(slot_register(QString,QString,QString)));//注册
    connect(p_login_windows, SIGNAL(signal_login(QString,QString)), this, SLOT(slot_login(QString,QString)));//登录
    connect(p_login_windows, SIGNAL(signal_close()), this, SLOT(slot_destory()));//关闭信号和销毁槽函数
    p_login_windows->show();


    //网络
    p_tcp_client_mediator = new TcpClientMediator;
    connect(p_tcp_client_mediator, SIGNAL(signal_data_ready(char* , int)), this, SLOT(slot_deal_data(char*, int)));
    if(!p_tcp_client_mediator->OpenNet()){
        QMessageBox::about(p_login_windows, "提示", "网络异常");
        slot_destory();
    }

}


void Kernel::slot_destory()
{
    //先隐藏，事件触发关闭
    if(p_login_windows){
        p_login_windows->hide();
        delete p_login_windows;
        p_login_windows = nullptr;
    }

    for (auto ite = chat_windows_map.begin(); ite != chat_windows_map.end(); ) {
        ChatDialog* chat = ite->second;
        ite = chat_windows_map.erase(ite);
        chat->hide();
        delete chat;
    }

    if(p_tcp_client_mediator){
        p_tcp_client_mediator->CloseNet();
        delete p_tcp_client_mediator;
        p_tcp_client_mediator = nullptr;
    }
    exit(0);
}

//----------------处理接收数据---------------------------------------------------------------
//分析协议，分发包
void Kernel::slot_deal_data(char *buff, int len)
{

    int type = *(int* )buff;
    qDebug()<<"deal data : "<<type;
    P_FUN p_fun = NET_PACK_MAP(type);
    qDebug()<<"type :"<<type;
    if(p_fun)
        (this->*p_fun)(buff, len);
}

//处理注册回复包
void Kernel::deal_register_rs(char *buff, int len)
{
    qDebug()<<"deal register rs package";
    //拆包
    S_TCP_REGISTER_RS *register_rs = (S_TCP_REGISTER_RS*)buff;

    switch (register_rs->result) {
    case user_is_exist:
        QMessageBox::about(p_login_windows, "注册提示", "该用户已存在，请重新输入其他手机号");
        break;
    case name_is_exist:
        QMessageBox::about(p_login_windows, "注册提示", "该用户名已存在，请重新输入其他用户名");
        break;
    case register_success:
        QMessageBox::about(p_login_windows, "注册提示", "注册成功");
        break;
    default:
        break;
    }
}

//处理回复的好友列表信息
void Kernel::deal_friend_rs(char *buff, int len)
{
    qDebug()<<"deal friend rs package";
    S_FRIEND_INFO* friend_rq = (S_FRIEND_INFO*)buff;

    //判断自身
    if(friend_rq->userid == user_id){
        p_main_windows->set_info(friend_rq);
        return;
    }
    if(friend_list_map.count(friend_rq->userid)){//有人更新状态
        UserChildWidget* item = friend_list_map[friend_rq->userid];
        if(item->self_info->state == 0 && friend_rq->state == 1){
            qDebug()<<"user online";
        }
        //更新信息
        item->set_info(friend_rq);
        if(chat_windows_map.count(friend_rq->userid)){
            ChatDialog* chat = chat_windows_map[friend_rq->userid];
            chat->slot_setInfo(friend_rq->userid, QString::fromStdString(friend_rq->name),friend_rq->icon, friend_rq->state);
        }
    }else{//没人添加控件，并添加到map
        UserChildWidget* item = new UserChildWidget;
        connect(item, SIGNAL(icon_clicked(int)), this, SLOT(slot_chat(int)));
        //创建聊天窗口
        ChatDialog* chat = new ChatDialog;
        //connect 发送聊天信息
        connect(chat, SIGNAL(SIG_SendChatMsg(int,QString)), this, SLOT(slot_send_chat_info(int,QString)));
        //发送文件
        connect(chat, SIGNAL(signal_file(int,QString)), this, SLOT(slot_send_file(int,QString)));
        //set chat dialog
        chat->slot_setInfo(friend_rq->userid, friend_rq->name, friend_rq->icon, friend_rq->state);
        //add map
        chat_windows_map[friend_rq->userid] = chat;

        item->set_info(friend_rq);
        p_main_windows->add_friend(item);
        friend_list_map[friend_rq->userid] = item;
    }

}

//处理添加好友回复
void Kernel::deal_add_rs(char *buff, int len)
{
    S_ADD_FRIEND_RS* add_friend_rs = (S_ADD_FRIEND_RS*)buff;
    int result = add_friend_rs->result;

    switch (result) {
    case no_this_user:
        QMessageBox::about(p_main_windows, "提示", "没有该用户");
        break;
    case user_offline:
        QMessageBox::about(p_main_windows, "提示", "用户离线");
        break;
    case add_success:
        QMessageBox::about(p_main_windows, "提示", "添加好友成功");
        break;
    case user_refuse:
        QMessageBox::about(p_main_windows, "提示", "对方拒绝了你的请求");
        break;
    default:
        break;
    }
}

//处理添加好友请求
void Kernel::deal_friend_rq(char *buff, int len)
{
    S_ADD_FRIEND_RQ* add_friend_rq = (S_ADD_FRIEND_RQ*)buff;
    qDebug()<<"recv add friend request >> name"<<add_friend_rq->name;
    qDebug()<<"recv add friend request >> friend_name"<<add_friend_rq->friend_name;
    S_ADD_FRIEND_RS add_friend_rs;
    strcpy_s(add_friend_rs.friend_name, add_friend_rq->friend_name);
    add_friend_rs.friend_id = user_id;
    //add_friend_rs.friend_id = 5;
    add_friend_rs.user_id = add_friend_rq->user_id;

    //同意或拒绝
    if(QMessageBox::question(p_main_windows, "添加好友提示", QString("用户[%1]添加你为好友，是否同意?").arg(add_friend_rq->name)) == QMessageBox::Yes){
        add_friend_rs.result = add_success;
    }else{
        add_friend_rs.result = user_refuse;
    }

    //回复
    p_tcp_client_mediator->SendData(0, (char*)&add_friend_rs, sizeof(add_friend_rs));
}

//接收聊天处理
void Kernel::deal_chat_rq(char *buff, int len)
{
    //拆包
    S_CHAT_RQ* chat_rq = (S_CHAT_RQ*)buff;
    //找到对应聊天窗口，user_id为对方
    if(chat_windows_map.count(chat_rq->userid)){
        ChatDialog* chat = chat_windows_map[chat_rq->userid];
        //显示内容
        chat->slot_setChatMsg(QString::fromStdString(chat_rq->content));
        chat->showNormal();
    }
}

//回复/不在线
void Kernel::deal_chat_rs(char *buff, int len)
{
    S_CHAT_RS* chat_rs = (S_CHAT_RS*)buff;

    //将不在线信息贴在窗口上，friend_id为对方
    if(chat_windows_map.count(chat_rs->friendid)){
        ChatDialog* chat = chat_windows_map[chat_rs->friendid];
        chat->offline();
        chat->showNormal();
    }
}

//处理离线请求
void Kernel::deal_offline_rq(char *buff, int len)
{
    //拆包
    S_TCP_OFFLINE* offline_rq = (S_TCP_OFFLINE*)buff;
    //找到该用户，设置离线
    if(friend_list_map.count(offline_rq->userid)){
        UserChildWidget* item = friend_list_map[offline_rq->userid];
        item->set_offline();
    }
    if(chat_windows_map.count(offline_rq->userid)){
        ChatDialog* chat = chat_windows_map[offline_rq->userid];
        chat->offline_repaint();
    }
}

void Kernel::deal_file_rq(char *buff, int len)
{
    //拆包
    S_FILE_INFO_RQ* file_info_rq = (S_FILE_INFO_RQ*)buff;
    S_FILE_INFO_RS file_info_rs;
    //写回复
    strcpy(file_info_rs.file_id, file_info_rq->file_id);
    file_info_rs.friend_id = file_info_rq->friend_id;
    file_info_rs.user_id = file_info_rq->user_id;

    if(friend_list_map.count(file_info_rq->user_id)){
        UserChildWidget* item = friend_list_map[file_info_rq->user_id];
        std::string user_name(item->self_info->name);
        //询问
        QString str_question = QString("用户[%1]发送文件[%2]\n大小:[%3]bytes\n是否接收? ")
                .arg(QString::fromStdString(user_name)).arg(QString::fromStdString(std::string(file_info_rq->file_name)))
                .arg(file_info_rq->file_size);
        if(QMessageBox::question(p_main_windows, "文件传输请求", str_question) == QMessageBox::Yes){
            //同意 保存路径，缓存文件信息
            QString save_path = QFileDialog::getSaveFileName(nullptr, "保存文件",
                                                             QString::fromStdString(std::string(file_info_rq->file_name)), "所有文件(*.*)");
            if(save_path.isEmpty()){
                //拒绝
                file_info_rs.result = file_refuse;
            }else{
                file_info_rs.result = file_accept;
//              /回复赋值
                S_FILE_INFO* p_info = new S_FILE_INFO;
                strcpy(p_info->file_id, file_info_rq->file_id);
                strcpy(p_info->file_name, file_info_rq->file_name);
                p_info->file_size = file_info_rq->file_size;
                p_info->friend_id = file_info_rq->friend_id;
                p_info->user_id = file_info_rq->user_id;
                //f_open打开文件接受 gb2312字符集
                UTF8TOGB2312(p_info->file_path, sizeof(p_info->file_path), save_path);
                //打开文件，开始读写
                fopen_s(&(p_info->p_file), p_info->file_path, "wb");
                p_info->pos = 0;
                //添加缓存map
                file_info_map[std::string(p_info->file_id)] = p_info;
            }

        }else{
            //拒绝
            file_info_rs.result = file_refuse;
        }
    }

    //返回结果
    p_tcp_client_mediator->SendData(0, (char*)&file_info_rs, sizeof(file_info_rs));
}

void Kernel::deal_file_rs(char *buff, int len)
{
    qDebug()<<"file rs";
    //拆包
    S_FILE_INFO_RS* file_info_rs = (S_FILE_INFO_RS*)buff;
    //拒绝
    if(file_info_rs->result == file_refuse){
        QMessageBox::about(p_main_windows, "提示", "用户拒绝接收文件");
    }
    //同意
    if(file_info_rs->result == file_accept){
        std::string file_id(file_info_rs->file_id);
        if(!file_info_map.count(file_id)){
            qDebug()<<"find map fail";
            return ;
        }
        S_FILE_INFO* p_info = file_info_map[file_info_rs->file_id];

        fopen_s(&(p_info->p_file), p_info->file_path, "rb");
        if(!(p_info->p_file)){
            qDebug()<<"find info fail";
        }
        while(1){
            S_FILE_BLOCK_RQ block_rq;
            strcpy(block_rq.file_id, file_info_rs->file_id);
            block_rq.user_id = file_info_rs->user_id;
            block_rq.friend_id = file_info_rs->friend_id;
            int res = fread(block_rq.file_content, 1, _DEF_CONTENT, p_info->p_file);
            block_rq.block_size = res;
            p_info->pos += res;
            //发文件快
            qDebug()<<"send file block";
            p_tcp_client_mediator->SendData(0, (char*)&block_rq, sizeof(block_rq));


            if(p_info->pos >= p_info->file_size){
                fclose(p_info->p_file);
                file_info_map.erase(file_id);
                delete p_info;
                break;
            }
        }
    }
}

void Kernel::deal_file_block_rq(char *buff, int len)
{
    qDebug()<<"deal file block rq";
    //拆包
    S_FILE_BLOCK_RQ* block_rq = (S_FILE_BLOCK_RQ*)buff;
    //根据id从map找到info
    std::string file_id(block_rq->file_id);
    if(file_info_map.count(file_id)){
        S_FILE_INFO* p_info = file_info_map[file_id];

        //写数据
        int res = fwrite(block_rq->file_content, 1, block_rq->block_size, p_info->p_file);

        p_info->pos += res;
        //回收
        if(p_info->pos >= p_info->file_size){
            fclose(p_info->p_file);
            file_info_map.erase(file_id);
            delete p_info;
        }
    }else{
        return;
    }

}

//处理登录回复包
void Kernel::deal_login_rs(char *buff, int len){
    qDebug()<<"deal login rs package";
    S_TCP_LOGIN_RS* login_rs = (S_TCP_LOGIN_RS*)buff;

    switch (login_rs->result) {
    case login_success:
        //QMessageBox::about(p_main_windows, "登陆提示", "登陆成功");
        user_id = login_rs->userid;
        p_main_windows->p_self_info->userid = user_id;
        //界面切换，使用模态对话框，点完之后切换页面
        p_main_windows->show();
        p_login_windows->hide();
        break;
    case user_not_exist:
        QMessageBox::about(p_login_windows, "登陆提示", "该用户不存在");
        break;
    case password_error:
        QMessageBox::about(p_login_windows, "登陆提示", "密码错误");
    default:
        break;
    }
}
//----------------处理接收数据----------------------------------------------------------



//----------------ui界面槽函数----------------------------------------------------------
//注册
void Kernel::slot_register(QString name, QString tel, QString password)
{
    qDebug()<<"slot_register";
    std::string std_tel = tel.toStdString();
    std::string std_name = name.toStdString();
    std::string std_password = password.toStdString();

    S_TCP_REGISTER_RQ register_request;
    strcpy(register_request.name, std_name.c_str());
    strcpy(register_request.telephone, std_tel.c_str());
    strcpy(register_request.password, std_password.c_str());
    //TODO 字符编码，加密优化（可做）
    p_tcp_client_mediator->SendData(0, (char*)&register_request, sizeof(register_request));
}

//登录
void Kernel::slot_login(QString tel, QString password)
{
    qDebug()<<"slot_login";
    std::string std_tel = tel.toStdString();
    std::string std_password = password.toStdString();

    S_TCP_LOGIN_RQ login_request;
    strcpy(login_request.telephone, std_tel.c_str());
    strcpy(login_request.password, std_password.c_str());
    //
    p_tcp_client_mediator->SendData(0, (char*)&login_request, sizeof(login_request));
}

//弹出聊天窗口
void Kernel::slot_chat(int id)
{
    if(chat_windows_map.count(id)){
        ChatDialog* chat = chat_windows_map[id];
        chat->showNormal();//可最小化显示
    }
}

//弹出添加好友框
void Kernel::slot_add_friend()
{
    qDebug()<<"slot add friend";
    //弹出输入窗口
    QInputDialog input(p_main_windows);
    bool isok = true;
    //QString name = input.getText(p_main_windows, "添加好友", "输入好友名称 :");
    QString name = input.getText(p_main_windows, "添加好友", "输入好友名称 :", QLineEdit::Normal, "", &isok, Qt::MSWindowsFixedSizeDialogHint);
    qDebug()<<"input :"<<name;
    //--输入判断
    if(name.isEmpty()) return;
    if(name.length() > 16) {
        QMessageBox::about(p_main_windows, "提示", "输入名字过长");
    }
    if(name == p_main_windows->p_self_info->name){
        QMessageBox::about(p_main_windows, "提示", "不能添加自己为好友");
    }
    for(auto ite = friend_list_map.begin(); ite != friend_list_map.end(); ++ite){
        UserChildWidget* item = ite->second;
        if(item->self_info->name == name){
            QMessageBox::about(p_main_windows, "提示", "该用户与你已经是好友");
            return;
        }
    }

    //--发送添加好友包
    S_ADD_FRIEND_RQ add_friend_rq;
    add_friend_rq.user_id = p_main_windows->p_self_info->userid;
    strcpy_s(add_friend_rq.name, sizeof(p_main_windows->p_self_info->name), p_main_windows->p_self_info->name);
    strcpy_s(add_friend_rq.friend_name, name.toStdString().c_str());
    qDebug()<<"send friend package >> user name :"<<add_friend_rq.name;
    qDebug()<<"send friend package >> friend name :"<<add_friend_rq.friend_name;
    p_tcp_client_mediator->SendData(0, (char*)&add_friend_rq, sizeof(add_friend_rq));
    qDebug()<<"send add friend rq";
}

//发送聊天信息
void Kernel::slot_send_chat_info(int id, QString content)
{
    S_CHAT_RQ chat_rq;
    chat_rq.userid = user_id;
    chat_rq.friendid = id;

    std::string std_content = content.toStdString();
    strcpy_s(chat_rq.content, std_content.c_str());

    p_tcp_client_mediator->SendData(0, (char*)&chat_rq, sizeof(chat_rq));
}

//发送离线请求
void Kernel::slot_offline()
{
    S_TCP_OFFLINE offline_rq;
    offline_rq.userid = user_id;

    p_tcp_client_mediator->SendData(0, (char*)&offline_rq, sizeof(offline_rq));

    slot_destory();
}
#include "QFileInfo"
#include "QTime"
//发送文件
void Kernel::slot_send_file(int id, QString path)
{
    //打包发送
    QFileInfo info(path);
    std::string file_name = info.fileName().toStdString();

    S_FILE_INFO_RQ file_send_rq;
    strcpy(file_send_rq.file_name, file_name.c_str());
    file_send_rq.file_size = info.size();
    QString time = QTime::currentTime().toString("hhmmsszzz");
    strcpy(file_send_rq.file_id,  time.toStdString().c_str());//可以使用MD5获得文件指纹，这里使用时间戳（并发量小，默认同一ms内只有一个文件被发送）
    file_send_rq.user_id = user_id;
    file_send_rq.friend_id = id;

    //rs
    S_FILE_INFO* p_info = new S_FILE_INFO;
    strcpy(p_info->file_id, file_send_rq.file_id);
    strcpy(p_info->file_name, file_send_rq.file_name);
    p_info->file_size = file_send_rq.file_size;
    strcpy(p_info->file_id, file_send_rq.file_id);
    p_info->p_file = nullptr;
    p_info->pos = 0;
    p_info->user_id = file_send_rq.user_id;
    p_info->friend_id = file_send_rq.friend_id;
    UTF8TOGB2312(p_info->file_path, sizeof(p_info->file_path), path);
    //存储map
    file_info_map[std::string(file_send_rq.file_id)] = p_info;

    p_tcp_client_mediator->SendData(0, (char*)&file_send_rq, sizeof(file_send_rq));
}
//----------------ui界面槽函数----------------------------------------------------------
