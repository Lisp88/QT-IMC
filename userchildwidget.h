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

    void set_info(S_FRIEND_INFO* friend_info);
private slots:
    void on_pb_icon_clicked();

private:
    Ui::UserChildWidget *ui;

public:
    S_FRIEND_INFO* self_info;//使用结构体简化传参
};

#endif // USERCHILDWIDGET_H
