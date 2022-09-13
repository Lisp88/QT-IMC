#include "DialogMain.h"
#include "ui_DialogMain.h"

DialogMain::DialogMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMain)
{
    ui->setupUi(this);
    setWindowTitle("IMCv1.0");
    p_layout = new QVBoxLayout;//父亲控件回收子控件
    p_layout->setContentsMargins(0, 0, 0, 0);//设置布局外边距
    p_layout->setSpacing(5);//设置控件间的距离，UI设置的是弹簧和控件距离

    ui->wdg_lst->setLayout(p_layout);//绑定布局到widget容器，ui界面画的widget控件属于父类框
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
}
