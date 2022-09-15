#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_SendChatMsg( int id , QString content);
public:
    explicit ChatDialog(QWidget *parent = 0);
    ~ChatDialog();

    void slot_setInfo( int id, QString name, int icon, int state);
    void slot_setChatMsg( QString content );
    void offline();
    void offline_repaint();
private slots:
    void on_pb_send_clicked();

private:
    Ui::ChatDialog *ui;
    int m_id;
    QString m_name;
    int m_icon;
};

#endif // CHATDIALOG_H
