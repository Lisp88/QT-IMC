#include "DialogMain.h"
#include "ui_DialogMain.h"
#include "QDebug"
DialogMain::DialogMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMain)
{
    ui->setupUi(this);

    p_self_info = new S_FRIEND_INFO;

    setWindowTitle("IMCv1.0");
    p_layout = new QVBoxLayout;//父亲控件回收子控件
    p_layout->setContentsMargins(0, 0, 0, 0);//设置布局外边距
    p_layout->setSpacing(5);//设置控件间的距离，UI设置的是弹簧和控件距离

    ui->wdg_lst->setLayout(p_layout);//绑定布局到widget容器，ui界面画的widget控件属于父类框

    m_menu = new QMenu(this);
    //添加菜单项
    m_menu->addAction("添加好友");
    m_menu->addAction("系统设置");

    //添加响应，信号为菜单触发
    connect(m_menu, SIGNAL(triggered(QAction*)), this, SLOT(slot_deal_menu(QAction*)));
}

DialogMain::~DialogMain()
{
    delete ui;
}

void DialogMain::add_friend(QWidget *user)
{
    p_layout->addWidget(user);//将user子控件添加到已布局完毕的父控件
}
//更新信息
void DialogMain::set_info(S_FRIEND_INFO *self_info)
{
    //更新UI
    QString path = QString(":/tx/%1.png").arg(self_info->icon);
    ui->pb_icon->setIcon(QIcon(path));

    ui->lb_name->setText(self_info->name);
    ui->le_feeling->setText(self_info->feeling);
    //更新自身
    p_self_info->userid = self_info->userid;
    p_self_info->icon = self_info->icon;
    strcpy_s(p_self_info->name, sizeof(self_info->name), self_info->name);
    strcpy_s(p_self_info->feeling, sizeof(self_info->feeling), self_info->feeling);
    p_self_info->state = self_info->state;
}

void DialogMain::slot_deal_menu(QAction *action)
{
    if(action->text() == "添加好友"){
        qDebug()<<"menu add friend";
        Q_EMIT signal_add_friend();
    }else if(action->text() == "系统设置"){
        qDebug()<<"meun system set";
    }
}
//点击菜单触发槽
void DialogMain::on_pb_menu_clicked()
{
    //m_menu->exec(QCursor::pos());//弹出菜单，以当前鼠标位置弹出
    //修改弹出位置，向上弹出（y偏移sizeHint个位置）
    QPoint p(QCursor::pos().x(), QCursor::pos().y()-m_menu->sizeHint().height());//sizeHint
    m_menu->exec(p);
}

