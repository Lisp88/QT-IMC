#ifndef USERCHILDWIDGET_H
#define USERCHILDWIDGET_H

#include <QWidget>
#include "Net/Packdef.h"

namespace Ui {
class UserChildWidget;
}

class UserChildWidget : public QWidget
{
    Q_OBJECT

signals:
    void icon_clicked(int id);
public:
    explicit UserChildWidget(QWidget *parent = nullptr);
    ~UserChildWidget();
    //设置信息
    void set_info(S_FRIEND_INFO* friend_info);
    //设置离线
    void set_offline();
    //添加事件
    void mouseDoubleClickEvent();
private slots:
    void on_pb_icon_clicked();//鼠标点击头像槽函数



private:
    Ui::UserChildWidget *ui;

public:
    S_FRIEND_INFO* self_info;//使用结构体简化传参
};

#endif // USERCHILDWIDGET_H
