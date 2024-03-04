#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include "Singleton.h"
#include "tcpclient.h"
#include "json.hpp"
#include "Msg.h"
#include "packageprocessor.h"
#include <memory>
#include <QStringListModel>
#include "chatwindow.h"
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    int m_userId = 0;
    std::string m_username;
    std::unique_ptr<QStringListModel> model_ptr;


    std::unique_ptr<ChatWindow> chatWindow_ptr;
    std::unique_ptr<QVBoxLayout> chatLayout_ptr;

    void processReceivedData(QByteArray data);

    //发送登陆请求
    void on_loginButton_clicked();

    //处理用户发送的数据
    void on_plainTextEdit_ReturnPressed();

    //Business Logic
    void sendMsg_userLogin();
    void getMsg_userLogin(std::string data);

    void sendMsg_getUserList();
    void getMsg_getUserList(std::string data);

    void sendMsg_getMsgs();
    void getMsg_getMsgs(std::string data);

    void getMsg_getChatMsg(std::string data);
};
#endif // WIDGET_H
