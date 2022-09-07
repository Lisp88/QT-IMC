#include "kernel.h"
#include <QMessageBox>
#include <QDebug>
#include "Packdef.h"
//#include "cstring"
#define NET_PACK_MAP(n)         net_pack_map[n-_DEF_PROTOCOL_BASE]

void Kernel::set_net_pack_map() {
    //NET_PACK_MAP(_DEF_TCP_LOGIN_RQ) = &Kernel::deal_login_data;
    NET_PACK_MAP(_DEF_TCP_REGISTER_RQ) = &Kernel::deal_register_rs;
}

Kernel::Kernel(QObject *parent) : QObject(parent)
{
    //为map赋值
    set_net_pack_map();
    //窗体
    p_main_windows = new MyChatDialog;
    //信号槽绑定
    connect(p_main_windows, SIGNAL(signal_close()), this, SLOT(slot_destory()));//关闭信号和销毁槽函数
    connect(p_main_windows, SIGNAL(signal_register(QString,QString,QString)), this, SLOT(slot_register(QString,QString,QString)));//注册
    connect(p_main_windows, SIGNAL(signal_login(QString,QString)), this, SLOT(slot_login(QString,QString)));//登录
    p_main_windows->show();

    //网络
    p_tcp_client_mediator = new TcpClientMediator;
    connect(p_tcp_client_mediator, SIGNAL(signal_data_ready(char* , int)), this, SLOT(slot_deal_data(char*, int)));
    if(!p_tcp_client_mediator->OpenNet()){
        QMessageBox::about(p_main_windows, "提示", "网络异常");
        slot_destory();
    }
}


void Kernel::slot_destory()
{
    //先隐藏，事件触发关闭
    if(p_main_windows){
        p_main_windows->hide();
        delete p_main_windows;
        p_main_windows = nullptr;
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
    qDebug()<<"deal data"<<buff;
    int type = *(int* )buff;
    P_FUN p_fun = NET_PACK_MAP(type);

    if(p_fun)
        (this->*p_fun)(buff, len);
}

//处理注册回复包
void Kernel::deal_register_rs(char *buff, int len)
{
    qDebug()<<"deal register rs package";
}
//----------------处理接收数据---------------------


//----------------ui界面槽函数---------------------
//注册
void Kernel::slot_register(QString name, QString tel, QString password)
{
    qDebug()<<"slot_register";
    std::string std_tel = tel.toStdString();
    std::string std_name = tel.toStdString();
    std::string std_password = tel.toStdString();

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
    std::string std_password = tel.toStdString();

    S_TCP_LOGIN_RQ login_request;
    strcpy(login_request.telephone, std_tel.c_str());
    strcpy(login_request.password, std_password.c_str());
    //
    p_tcp_client_mediator->SendData(0, (char*)&login_request, sizeof(login_request));
}
//----------------ui界面槽函数---------------------
