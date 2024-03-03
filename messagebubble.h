#ifndef MESSAGEBUBBLE_H
#define MESSAGEBUBBLE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class MessageBubble : public QWidget
{
    Q_OBJECT
public:
    explicit MessageBubble(const QString &message, const QString &username, bool alignRight, QWidget *parent = nullptr);
    void setAlignment(Qt::Alignment alignment);
private:
    QLabel *messageLabel;
    QLabel *usernameLabel;
};

#endif // MESSAGEBUBBLE_H
