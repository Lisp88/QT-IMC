#include <QDialog>
#include <QCloseEvent>

#ifndef MYCHATDIALOG_H
#define MYCHATDIALOG_H


QT_BEGIN_NAMESPACE
namespace Ui { class MyChatDialog; }
QT_END_NAMESPACE

class MyChatDialog : public QDialog
{
    Q_OBJECT
signals:
    void signal_close();//关闭信号
    void signal_register(QString name, QString tel, QString password);//注册信号
    void signal_login(QString name, QString password);
public:
    MyChatDialog(QWidget *parent = nullptr);
    ~MyChatDialog();
    //关闭窗口 事件
    void close_event(QCloseEvent* event);
private slots:
    void on_pb_login_clicked();

    void on_pb_login_exit_clicked();

    void on_pb_register_clicked();

    void on_pb_register_exit_clicked();

private:
    Ui::MyChatDialog *ui;
};
#endif // MYCHATDIALOG_H
