#include "INet.h"
#include "INetMediator.h"
#include "DialogLogin.h"
#include "DialogMain.h"
#include "TcpClientMediator.h"
#include "qtextcodec.h"
#include "userchildwidget.h"
#include "chatdialog.h"
#ifndef KERNEL_H
#define KERNEL_H
class Kernel : public QObject
{
    Q_OBJECT
public:
    explicit Kernel(QObject *parent = nullptr);

    //处理回复包
    //登录
    void deal_login_rs(char* buff, int len);
    //注册
    void deal_register_rs(char* buff, int len);
    //好友
    void deal_friend_rs(char* buff, int len);
    //添加回复
    void deal_add_rs(char* buff, int len);
    //添加请求
    void deal_friend_rq(char* buff, int len);
    //聊天请求
    void deal_chat_rq(char* buff, int len);
    //聊天回复（不在线）
    void deal_chat_rs(char* buff, int len);
    //处理离线请求
    void deal_offline_rq(char* buff, int len);
signals:

public slots:
    void slot_destory();//回收窗体
    void slot_deal_data(char* buff, int len);//处理接收到的数据
    void slot_register(QString name, QString tel, QString password);//注册槽函数
    void slot_login(QString tel, QString password);//登录槽函数
    void slot_chat(int id);//聊天槽函数
    void slot_add_friend();//添加好友
    void slot_send_chat_info(int id, QString content);//发送聊天信息
    void slot_offline();//离线
private:
    DialogLogin* p_login_windows;
    DialogMain* p_main_windows;
    INetMediator* p_tcp_client_mediator;
    typedef void (Kernel::*P_FUN)(char*, int);
    std::map<int, P_FUN> net_pack_map;//协议包映射处理函数
    std::map<int, UserChildWidget*> friend_list_map;//好友id映射ui控件
    std::map<int, ChatDialog*> chat_windows_map;//好友id映射聊天窗口
    int user_id;

    void set_net_pack_map();//工具函数，为map进行赋值
public:
    //字符编码转换，不依赖对象设静态
    static void UTF8TOGB2312(char* gbbuff, int len, QString &utf8){
            //转码对象
            QTextCodec* gb2312code = QTextCodec::codecForName("gb2312");//表示国标2312的字符编码对象
            QByteArray byte_array = gb2312code->fromUnicode(utf8);//通过gb2312的编码对象调用API将utf8中的字符转为gb形式存储刀字节数组中

            strcpy_s(gbbuff, len, byte_array.data());//返回结果
    }
    static QString GB2312ToUtf8(char* gbbuff){
        QTextCodec* gb2312code = QTextCodec::codecForName("gb2312");
        return gb2312code->toUnicode(gbbuff);
    }
};

#endif // KERNEL_H
