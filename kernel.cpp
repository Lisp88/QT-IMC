#include "kernel.h"
#include <QMessageBox>
#include <QDebug>
Kernel::Kernel(QObject *parent) : QObject(parent)
{
    //窗体
    p_main_windows = new MyChatDialog;
    //信号槽绑定
    connect(p_main_windows, SIGNAL(close_signal()), this, SLOT(slot_destory()));
    p_main_windows->show();

    //网络
    p_tcp_client_mediator = new TcpClientMediator;
    connect(p_tcp_client_mediator, SIGNAL(signal_data_ready(char* , int)), this, SLOT(slot_deal_data(char*, int)));
    if(!p_tcp_client_mediator->OpenNet()){
        QMessageBox::about(p_main_windows, "提示", "网络异常");
        slot_destory();
    }

    char buff[]{"hello server"};
    p_tcp_client_mediator->SendData(0, buff, strlen(buff)+1);
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

void Kernel::slot_deal_data(char *buff, int len)
{
    qDebug()<<"server : "<<buff;
}
