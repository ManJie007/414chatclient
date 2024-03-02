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
    std::unique_ptr<QStringListModel> model_ptr;

    void processReceivedData(QByteArray data);
    void on_loginButton_clicked();

    //Business Logic
    void sendMsg_userLogin();
    void getMsg_userLogin(std::string data);

    void sendMsg_getUserList();
    void getMsg_getUserList(std::string data);
};
#endif // WIDGET_H
