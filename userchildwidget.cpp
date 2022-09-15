#include "userchildwidget.h"
#include "ui_userchildwidget.h"
UserChildWidget::UserChildWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserChildWidget)
{
    ui->setupUi(this);
    self_info = new S_FRIEND_INFO;
    ui->lb_feeling->setAlignment(Qt::AlignHCenter);
}

UserChildWidget::~UserChildWidget()
{
    delete ui;
}
#include <QBitmap>
void UserChildWidget::set_info(S_FRIEND_INFO* friend_info)
{
    strcpy_s(self_info->feeling, sizeof(friend_info->feeling), friend_info->feeling);
    strcpy_s(self_info->name,sizeof(friend_info->name), friend_info->name);
    self_info->icon = friend_info->icon;
    self_info->state = friend_info->state;
    self_info->type = friend_info->type;
    self_info->userid = friend_info->userid;
    //ui更新
    QString name(self_info->name);
    ui->lb_name->setText(name);
    QString feeling(self_info->feeling);
    ui->lb_feeling->setText(feeling);
//    对资源的引用 :/
    QString path = QString(":/tx/%1.png").arg(self_info->icon);

    //在线彩色
    if(self_info->state || self_info->userid == 2)
        ui->pb_icon->setIcon(QIcon(path));
    else{
        QBitmap bmp;
        bmp.load(path);
        ui->pb_icon->setIcon(bmp);
    }
    //重绘
    this->repaint();
}

void UserChildWidget::set_offline()
{
    QString path = QString(":/tx/%1.png").arg(self_info->icon);
    QBitmap bmp;
    bmp.load(path);
    ui->pb_icon->setIcon(bmp);
    this->repaint();
}

void UserChildWidget::mouseDoubleClickEvent()
{
    Q_EMIT icon_clicked(self_info->userid);
}
//头像槽
void UserChildWidget::on_pb_icon_clicked()
{
    Q_EMIT icon_clicked(self_info->userid);
}

