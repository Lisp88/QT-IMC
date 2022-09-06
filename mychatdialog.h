#ifndef MYCHATDIALOG_H
#define MYCHATDIALOG_H

#include <QDialog>
#include <QCloseEvent>


QT_BEGIN_NAMESPACE
namespace Ui { class MyChatDialog; }
QT_END_NAMESPACE

class MyChatDialog : public QDialog
{
    Q_OBJECT
signals:
    void signal_close();
public:
    MyChatDialog(QWidget *parent = nullptr);
    ~MyChatDialog();
    //关闭窗口 事件
    void close_event(QCloseEvent* event);
private:
    Ui::MyChatDialog *ui;
};
#endif // MYCHATDIALOG_H
