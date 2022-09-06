#ifndef KERNEL_H
#define KERNEL_H

#include "INet.h"
#include "INetMediator.h"
#include "mychatdialog.h"
#include "TcpClientMediator.h"
class Kernel : public QObject
{
    Q_OBJECT
public:
    explicit Kernel(QObject *parent = nullptr);

signals:

public slots:
    void slot_destory();//回收窗体
    void slot_deal_data(char* buff, int len);//处理接收到的数据
private:
    MyChatDialog* p_main_windows;
    INetMediator* p_tcp_client_mediator;
};

#endif // KERNEL_H
