#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include "Singleton.h"
#include "tcpclient.h"
#include "json.hpp"
#include "Msg.h"
#include "packageprocessor.h"

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

    void processReceivedData(QByteArray data);
    void on_loginButton_clicked();
};
#endif // WIDGET_H
