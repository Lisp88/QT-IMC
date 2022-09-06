#include "mychatdialog.h"
#include "ui_mychatdialog.h"
#include <QMessageBox>

MyChatDialog::MyChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MyChatDialog)
{
    ui->setupUi(this);
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

