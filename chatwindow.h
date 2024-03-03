#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QListWidget>
#include "messagebubble.h"

class ChatWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ChatWindow(QWidget *parent = nullptr);

    void addMessage(const QString &message, const QString &username, bool alignRight);
private:
    QListWidget *chatListWidget;
};

#endif // CHATWINDOW_H
