#include "INet.h"
#include "INetMediator.h"
#include "mychatdialog.h"
#include "TcpClientMediator.h"
#ifndef KERNEL_H
#define KERNEL_H
class Kernel : public QObject
{
    Q_OBJECT
public:
    explicit Kernel(QObject *parent = nullptr);

    //处理回复包
    //登录
    //注册
    void deal_register_rs(char* buff, int len);
signals:

public slots:
    void slot_destory();//回收窗体
    void slot_deal_data(char* buff, int len);//处理接收到的数据
    void slot_register(QString name, QString tel, QString password);//注册槽函数
    void slot_login(QString tel, QString password);//登录槽函数
private:
    MyChatDialog* p_main_windows;
    INetMediator* p_tcp_client_mediator;
    typedef void (Kernel::*P_FUN)(char*, int);
    std::map<int, P_FUN> net_pack_map;

    void set_net_pack_map();//工具函数，为map进行赋值
};

#endif // KERNEL_H
