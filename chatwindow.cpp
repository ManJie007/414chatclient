#include "chatwindow.h"

ChatWindow::ChatWindow(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    chatListWidget = new QListWidget(this);
    layout->addWidget(chatListWidget);
    // 设置边距为0
    layout->setContentsMargins(0, 0, 0, 0);

    // Create a few sample messages
    // for(int i = 0; i < 2; ++i) {
    //     addMessage("Hello, how are you?", "User1", true);
    //     addMessage("I'm good, thank you!", "User2", false);
    // }
}

void ChatWindow::addMessage(const QString &message, const QString &username, bool alignRight)
{
    MessageBubble *bubble = new MessageBubble(message, username, alignRight);
    QListWidgetItem *item = new QListWidgetItem(chatListWidget);
    item->setSizeHint(bubble->sizeHint());
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable); // Disable selection
    chatListWidget->addItem(item);
    chatListWidget->setItemWidget(item, bubble);
}
