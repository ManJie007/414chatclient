#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //QListView QStringListModel
    model_ptr.reset(new QStringListModel(this));
    ui->listView->setModel(model_ptr.get());
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //network
    Singleton<TcpClient>::Instance().connectToServer("172.16.229.128", 12345);
    connect(&Singleton<TcpClient>::Instance(), &TcpClient::dataReceived, this, &Widget::processReceivedData);
    connect(ui->pushButton_login, &QPushButton::clicked, this, &Widget::on_loginButton_clicked);

    //chatWindow
    chatWindow_ptr.reset(new ChatWindow(ui->widget_chat));
    chatLayout_ptr.reset(new QVBoxLayout(ui->widget_chat));
    // 设置边距为0
    chatLayout_ptr->setContentsMargins(0, 0, 0, 0);
    chatLayout_ptr->addWidget(chatWindow_ptr.get());
    ui->widget_chat->setLayout(chatLayout_ptr.get());

    //处理用户发送信息
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &Widget::on_plainTextEdit_ReturnPressed);
}

Widget::~Widget()
{
    delete ui;
    Singleton<TcpClient>::destroy();
}

void Widget::processReceivedData(QByteArray data)
{
    qDebug() << "Widget::processReceivedData() Received data:" << data;

    std::string inbuf = data.toStdString();
    net::BinaryStreamReader readStream(inbuf.c_str(), inbuf.length());
    int32_t cmd;
    if (!readStream.ReadInt32(cmd))
    {
        qDebug() << "Widget::processReceivedData() read cmd error";
        return;
    }

    int seq;
    if (!readStream.ReadInt32(seq))
    {
        qDebug() << "Widget::processReceivedData() read seq error";
        return;
    }

    std::string data_str;
    size_t datalength;
    if (!readStream.ReadString(&data_str, 0, datalength))
    {
        qDebug() << "Widget::processReceivedData() read data error";
        return;
    }

    qDebug() << "Widget::processReceivedData() received data : " << QString::fromStdString(data_str);

    using json = nlohmann::json;
    json j = json::parse(data_str);

    switch (cmd) {
    case msg_type_login:
        if(j["code"] == 0) {
            getMsg_userLogin(data_str);
            sendMsg_getUserList();
            sendMsg_getMsgs();
        }
        break;
    case msg_type_getuserlist:
        if(j["code"] == 0) {
            getMsg_getUserList(data_str);
        }
        break;
    case msg_type_getMsgs:
        if(j["code"] == 0) {
            getMsg_getMsgs(data_str);
        }
        break;
    case msg_type_chat:
        if(j["code"] == 0) {
            getMsg_getChatMsg(data_str);
        }
        break;
    default:
        break;
    }
}

void Widget::on_loginButton_clicked()
{
    sendMsg_userLogin();
}

void Widget::on_plainTextEdit_ReturnPressed()
{
    // 获取文本框的内容
    QString text = ui->plainTextEdit->toPlainText();

    // 检查最后一个字符是否为回车键
    if (!text.isEmpty() && text.right(1) == "\n") {
        qDebug() << "Return pressed! Text entered:" << text.trimmed();
        // 在这里执行您想要的操作，比如处理输入的文本
        using json = nlohmann::json;

        json j;

        j["senderid"] = m_userId;
        j["senderusername"] = m_username;
        j["msgcontent"] = text.toStdString();
        j["sendtime_str"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();

        std::string data = PackageProcessor::getSendPackageData(msg_type_chat, 0, j.dump());
        qDebug() << "Widget::on_plainTextEdit_ReturnPressed() sendData : " << QString::fromStdString(data);
        Singleton<TcpClient>::Instance().sendData(QByteArray(data.c_str(), data.length()));

        // 清空文本框
        ui->plainTextEdit->clear();
    }
}

void Widget::sendMsg_userLogin()
{
    using json = nlohmann::json;

    json j;

    j["username"] = ui->lineEdit_username->text().toStdString();
    j["password"] = ui->lineEdit_passwd->text().toStdString();

    std::string data = PackageProcessor::getSendPackageData(msg_type_login, 0, j.dump());
    qDebug() << "Widget::on_loginButton_clicked() sendData : " << QString::fromStdString(data);
    Singleton<TcpClient>::Instance().sendData(QByteArray(data.c_str(), data.length()));
}

void Widget::getMsg_userLogin(std::string data)
{
    using json = nlohmann::json;

    json msg = json::parse(data);
    m_userId = msg["userid"];
    m_username = msg["username"];
    qDebug() << "Widget::processReceivedData() 登陆成功 " << "userid : " << m_userId << " username : " << QString::fromStdString(msg["username"]);
}

void Widget::sendMsg_getUserList()
{
    qDebug() << "Widget::sendMsg_getUserList() send Msg to get userlist";
    std::string str;
    std::string data = PackageProcessor::getSendPackageData(msg_type_getuserlist, 0, str);
    qDebug() << "Widget::sendMsg_getUserList() sendData : " << QString::fromStdString(data);
    Singleton<TcpClient>::Instance().sendData(QByteArray(data.c_str(), data.length()));
}

void Widget::getMsg_getUserList(std::string data)
{
    using json = nlohmann::json;

    json msg = json::parse(data);
    for (auto &user : msg["userlistinfo"]) {
        int userid = user["userid"];
        std::string username = user["username"];
        model_ptr->insertRow(model_ptr->rowCount());
        QModelIndex index = model_ptr->index(model_ptr->rowCount() - 1, 0);

        QString username_qstring = QString::fromStdString(username);
        if(userid == m_userId) {
            username_qstring += " (当前用户) ";
        }
        model_ptr->setData(index, username_qstring, Qt::DisplayRole);
    }
}

void Widget::sendMsg_getMsgs()
{
    qDebug() << "Widget::sendMsg_getMsgs() send Msg to get Msgs";
    std::string str;
    std::string data = PackageProcessor::getSendPackageData(msg_type_getMsgs, 0, str);
    qDebug() << "Widget::sendMsg_getMsgs() sendData : " << QString::fromStdString(data);
    Singleton<TcpClient>::Instance().sendData(QByteArray(data.c_str(), data.length()));
}

void Widget::getMsg_getMsgs(std::string data)
{
    using json = nlohmann::json;

    json msg = json::parse(data);
    for (auto &oneMsg : msg["msgs"]) {
        int sendid = oneMsg["senderid"];
        std::string senderusername = oneMsg["senderusername"];
        std::string msgcontent = oneMsg["msgcontent"];
        std::string sendtime_str = oneMsg["sendtime_str"];

        QString nameAtime = QString("%1 %2").arg(QString::fromStdString(senderusername)).arg(QString::fromStdString(sendtime_str));
        QString msgcontent_qstr = QString::fromStdString(msgcontent);

        if(sendid == m_userId) {
            chatWindow_ptr->addMessage(msgcontent_qstr, nameAtime, true);
        }else{
            chatWindow_ptr->addMessage(msgcontent_qstr, nameAtime, false);
        }
    }
}

void Widget::getMsg_getChatMsg(std::string data)
{
    using json = nlohmann::json;

    json msg = json::parse(data)["msg"];
    int senderid = msg["senderid"];
    std::string senderusername = msg["senderusername"];
    std::string msgcontent = msg["msgcontent"];
    std::string sendtime_str = msg["sendtime_str"];

    QString nameAtime = QString("%1 %2").arg(QString::fromStdString(senderusername)).arg(QString::fromStdString(sendtime_str));
    QString msgcontent_qstr = QString::fromStdString(msgcontent);

    if(senderid == m_userId) {
        chatWindow_ptr->addMessage(msgcontent_qstr, nameAtime, true);
    }else{
        chatWindow_ptr->addMessage(msgcontent_qstr, nameAtime, false);
    }
}
