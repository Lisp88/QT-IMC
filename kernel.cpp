#include "kernel.h"
#include <QMessageBox>
#include <QDebug>
#include "Packdef.h"
#include "userchildwidget.h"

#define NET_PACK_MAP(n)         net_pack_map[n-_DEF_PROTOCOL_BASE]

void Kernel::set_net_pack_map() {
    //NET_PACK_MAP(_DEF_TCP_LOGIN_RQ) = &Kernel::deal_login_data;
    NET_PACK_MAP(_DEF_TCP_REGISTER_RS) = &Kernel::deal_register_rs;
    NET_PACK_MAP(_DEF_TCP_LOGIN_RS) = &Kernel::deal_login_rs;
    NET_PACK_MAP(_DEF_TCP_FRIEND_INFO) = &Kernel::deal_friend_rs;
}

Kernel::Kernel(QObject *parent) : QObject(parent), user_id(0)
{
    //为map赋值
    set_net_pack_map();
    //窗体
    p_login_windows = new DialogLogin;
    p_main_windows = new DialogMain;
    //信号槽绑定
    connect(p_login_windows, SIGNAL(signal_close()), this, SLOT(slot_destory()));//关闭信号和销毁槽函数
    connect(p_login_windows, SIGNAL(signal_register(QString,QString,QString)), this, SLOT(slot_register(QString,QString,QString)));//注册
    connect(p_login_windows, SIGNAL(signal_login(QString,QString)), this, SLOT(slot_login(QString,QString)));//登录
    p_login_windows->show();

    //网络
    p_tcp_client_mediator = new TcpClientMediator;
    connect(p_tcp_client_mediator, SIGNAL(signal_data_ready(char* , int)), this, SLOT(slot_deal_data(char*, int)));
    if(!p_tcp_client_mediator->OpenNet()){
        QMessageBox::about(p_login_windows, "提示", "网络异常");
        slot_destory();
    }

//    //测试好友
//    for (int var = 0; var < 10; ++var) {
//        UserChildWidget* friend_item = new UserChildWidget;
//        p_main_windows->add_friend(friend_item);
//    }

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

//----------------处理接收数据---------------------
//分析协议，分发包
void Kernel::slot_deal_data(char *buff, int len)
{

    int type = *(int* )buff;
    qDebug()<<"deal data : "<<type;
    P_FUN p_fun = NET_PACK_MAP(type);

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
//#include "QDebug"
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
    }else{//没人添加控件，并添加到map
        UserChildWidget* item = new UserChildWidget;
        connect(item, SIGNAL(icon_clicked(int)), this, SLOT(slot_chat(int)));
        //创建聊天窗口
        ChatDialog* chat = new ChatDialog;
        //connect
        //set chat dialog
        chat->slot_setInfo(friend_rq->id, friend_rq->name, friend_rq->icon, friend_rq->state);
        //add map
        chat_windows_map[friend_rq->id] = chat;

        item->set_info(friend_rq);
        p_main_windows->add_friend(item);
        friend_list_map[friend_rq->userid] = item;
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
//----------------处理接收数据---------------------


//----------------ui界面槽函数---------------------
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
//----------------ui界面槽函数---------------------
