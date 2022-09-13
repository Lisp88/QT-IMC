#ifndef DIALOGMAIN_H
#define DIALOGMAIN_H

#include <QDialog>

#include "QVBoxLayout"//添加布局层
#include "Net/Packdef.h"
namespace Ui {
class DialogMain;
}

class DialogMain : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMain(QWidget *parent = nullptr);
    ~DialogMain();

    //添加好友列表
    void add_friend(QWidget* user);

    //设置信息
    void set_info(S_FRIEND_INFO* self_info);

private:
    Ui::DialogMain *ui;
    QVBoxLayout* p_layout;
    S_FRIEND_INFO* p_self_info;
};

#endif // DIALOGMAIN_H
