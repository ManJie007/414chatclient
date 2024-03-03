#include "messagebubble.h"

MessageBubble::MessageBubble(const QString &message, const QString &username, bool alignRight, QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5); // Set margins for spacing
    layout->setSpacing(5);

    // QLabel for user name
    usernameLabel = new QLabel(username, this);
    usernameLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(usernameLabel);

    // QLabel for message content (could be QTextBrowser for rich text)
    messageLabel = new QLabel(message, this);
    messageLabel->setWordWrap(true); // Wrap text
    layout->addWidget(messageLabel);


    // Spacer for layout alignment
    layout->addStretch();

    // Set stylesheet for bubble appearance
    // setStyleSheet("background-color: #000000; border-radius: 10px;");
    setStyleSheet("border-radius: 10px;");

    // Set default alignment to right (sender's messages)
    setAlignment(alignRight ? Qt::AlignRight : Qt::AlignLeft);
}

void MessageBubble::setAlignment(Qt::Alignment alignment)
{
    layout()->setAlignment(usernameLabel, alignment);
    layout()->setAlignment(messageLabel, alignment);
}
