#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

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

    if(j["code"].is_null()) {
        qDebug() << "Widget::processReceivedData() can't parse data";
        return;
    }

    int code = j["code"];
    switch (code) {
    case code_login_success:
        qDebug() << "Widget::processReceivedData() 登陆成功";
        break;
    default:
        break;
    }
}

void Widget::on_loginButton_clicked()
{
    using json = nlohmann::json;

    json j;

    j["username"] = ui->lineEdit_username->text().toStdString();
    j["password"] = ui->lineEdit_passwd->text().toStdString();

    std::string data = PackageProcessor::getSendPackageData(msg_type_login, 0, j.dump());
    qDebug() << "Widget::on_loginButton_clicked() sendData : " << QString::fromStdString(data);
    Singleton<TcpClient>::Instance().sendData(QByteArray(data.c_str(), data.length()));

}
