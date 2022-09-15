#include <QDialog>
#include <QCloseEvent>
#ifndef DIALOGLOGIN_H
#define DIALOGLOGIN_H


QT_BEGIN_NAMESPACE
namespace Ui { class DialogLogin; }
QT_END_NAMESPACE

class DialogLogin : public QDialog
{
    Q_OBJECT
signals:
    void signal_close();//关闭登录页面
    void signal_register(QString name, QString tel, QString password);//接收页面注册响应
    void signal_login(QString name, QString password);//接收页面登录响应
public:
    DialogLogin(QWidget *parent = nullptr);
    ~DialogLogin();

    //事件---
    //关闭页面的事件
    void closeEvent(QCloseEvent *event);


private slots:
    void on_pb_login_clicked();

    void on_pb_login_exit_clicked();

    void on_pb_register_clicked();

    void on_pb_register_exit_clicked();

private:
    Ui::DialogLogin* ui;
};
#endif // DIALOGLOGIN_H
