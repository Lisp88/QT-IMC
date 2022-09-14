#include "chatdialog.h"
#include "ui_chatdialog.h"
#include<QTime>
#include "QIcon"
#include "QBitmap"
ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

//设置
void ChatDialog::slot_setInfo(int id, QString name, int icon, int state)
{
    //设置成员
    m_id = id;
    m_name = name;
    ui->lb_name->setText(m_name);
    m_icon = icon;
    QString path = QString(":/tx/%1.png").arg(m_icon);

    //在线或机器人彩色
    if(state || id == 2)
        ui->pb_icon->setIcon(QIcon(path));
    else{
        QBitmap b;
        b.load(path);
        ui->pb_icon->setIcon(b);
    }
    //重绘
    this->repaint();
    //设置ui
    setWindowTitle( QString("与[%1]的聊天").arg( m_name ) );
}

//设置聊天信息
void ChatDialog::slot_setChatMsg(QString content)
{
    //添加到浏览 append 换行添加文本  [ 192.168.1.1 ] 10:51:20
    ui->tb_chat->append( QString("[%1] %2").arg(m_name)
                         .arg(QTime::currentTime().toString("hh:mm:ss")) );

    ui->tb_chat->append( content );
}
//用户离线
void ChatDialog::offline()
{
    ui->tb_chat->append( QString("[%1] %2 用户离线").arg(m_name)
                         .arg(QTime::currentTime().toString("hh:mm:ss")) );
}


//发送按钮
void ChatDialog::on_pb_send_clicked()
{
    //获取纯文本
    QString strContent = ui->te_chat->toPlainText();
    //空文本不发送
    if( strContent.isEmpty() ) return;  //isEmpty == ""
    //都是空格不发送
    if( strContent.remove(" ").isEmpty() ) return; //remove会改变strContent
    //tohtml 会带着文本样式
    strContent = ui->te_chat->toHtml();
    //清空编辑
    ui->te_chat->clear();
    //添加到浏览 append 换行添加文本  [ 我 ] 10:51:20
    ui->tb_chat->append( QString("[我] %1")
                         .arg(QTime::currentTime().toString("hh:mm:ss")) );

    ui->tb_chat->append( strContent );

    Q_EMIT SIG_SendChatMsg( m_id , strContent );
}
