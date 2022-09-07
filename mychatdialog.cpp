#include "mychatdialog.h"
#include "ui_mychatdialog.h"
#include <QMessageBox>
#include "regex"
MyChatDialog::MyChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MyChatDialog)
{
    ui->setupUi(this);

    ui->wg_lr->setCurrentIndex(0);//默认显示登录页面

}

MyChatDialog::~MyChatDialog()
{
    delete ui;
}

void MyChatDialog::close_event(QCloseEvent *event)
{
    //询问Box，父窗口指针，标题和提示语
    if(QMessageBox::question(this, "退出提示", "是否退出?") == QMessageBox::Yes){
        Q_EMIT signal_close();
    }
}

//按钮槽函数
//登录
void MyChatDialog::on_pb_login_clicked()
{
    QString tel = ui->le_login_tel->text();
    QString password = ui->le_login_password->text();
    //电话
    if(tel.isEmpty()){
        QMessageBox::about(this, "提示", "电话不允许为空！");
        return;
    }else{
        //正则判断
        std::regex reg("^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\\d{8}");
        std::smatch sm;
        std::string std_tel = tel.toStdString();
        if(!std::regex_match(std_tel, sm, reg)){
            QMessageBox::about(this, "提示", "手机号格式不正确！");
            return;
        }
    }
    if(password.isEmpty()){
        QMessageBox::about(this, "提示", "密码不允许为空！");
        return;
    }else if(password.length() > 16){
        QMessageBox::about(this, "提示", "密码长度不能超过十六个字符!");
        return;
    }
    Q_EMIT signal_login(tel, password);
}

//登录清空
void MyChatDialog::on_pb_login_exit_clicked()
{
    ui->le_login_tel->clear();
    ui->le_login_password->clear();
}

//注册
void MyChatDialog::on_pb_register_clicked()
{
    QString name = ui->le_register_user->text();
    QString tel = ui->le_register_tel->text();
    QString password = ui->le_register_password->text();
    //判断合法
    //用户名
    if(name.isEmpty()){
        QMessageBox::about(this, "提示", "用户名不允许为空！");
        return;
    }else if(name.length() > 10){
        QMessageBox::about(this, "提示", "用户名长度不能超过十个字符!");
        return;
    }
    //电话
    if(tel.isEmpty()){
        QMessageBox::about(this, "提示", "电话不允许为空！");
        return;
    }else{
        //正则判断
        std::string std_tel = tel.toStdString();
        std::regex reg("^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\\d{8}");
        std::smatch sm;
        if(!std::regex_match(std_tel, sm, reg)){
            QMessageBox::about(this, "提示", "手机号格式不正确！");
            return;
        }
    }
   //密码
    if(password.isEmpty()){
        QMessageBox::about(this, "提示", "密码不允许为空！");
        return;
    }else if(password.length() > 16){
        QMessageBox::about(this, "提示", "密码长度不能超过十六个字符!");
        return;
    }
    if(password != ui->le_register_repeat_password->text()){
        QMessageBox::about(this, "提示", "两次密码书写不一致！");
        return;
    }
    //发送注册信号
    Q_EMIT signal_register(name, tel, password);
}

//注册清空
void MyChatDialog::on_pb_register_exit_clicked()
{
    ui->le_register_password->clear();
    ui->le_register_user->clear();
    ui->le_register_repeat_password->clear();
    ui->le_register_tel->clear();
}

