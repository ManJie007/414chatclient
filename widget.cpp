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
        }
        break;
    case msg_type_getuserlist:
        if(j["code"] == 0) {
            getMsg_getUserList(data_str);
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
        std::string username = user["username"];
        model_ptr->insertRow(model_ptr->rowCount());
        QModelIndex index = model_ptr->index(model_ptr->rowCount() - 1, 0);
        model_ptr->setData(index, QString::fromStdString(username), Qt::DisplayRole);
    }
}
