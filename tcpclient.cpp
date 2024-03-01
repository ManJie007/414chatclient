#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent)
    : QObject{parent}, socket(nullptr)
{

}

TcpClient::~TcpClient()
{
    if (socket) {
        // 断开连接并销毁socket对象
        socket->disconnectFromHost();
        socket->deleteLater();
        socket = nullptr;
    }
}

void TcpClient::connectToServer(const QString &address, quint16 port)
{
    if (socket && socket->state() != QAbstractSocket::UnconnectedState)
    {
        qDebug() << "Already connected to a server";
        return;
    }

    socket.reset(new QTcpSocket(this));

    connect(socket.get(), &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket.get(), &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket.get(), &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(socket.get(), QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &TcpClient::onError);

    socket->connectToHost(address, port);
}

bool TcpClient::isConnected() const
{
    return socket && socket->state() == QAbstractSocket::ConnectedState;
}

void TcpClient::sendData(const QByteArray &data)
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState)
    {
        qDebug() << "Not connected to a server";
        return;
    }

    socket->write(data);
}

void TcpClient::onConnected()
{
    qDebug() << "Connected to server";
    emit connected();
}

void TcpClient::onDisconnected()
{
    qDebug() << "Disconnected from server";
    emit disconnected();
}

void TcpClient::onReadyRead()
{
    qint64 bytesAvailable = socket->bytesAvailable();
    // 不够一个包头大小
    if (bytesAvailable < (size_t)sizeof(chat_msg_header))
    {
        qDebug() << "buffer is not enough for a package header, socket->bytesAvailable() is " << bytesAvailable;
        return;
    }

    //取包头信息
    chat_msg_header header;
    socket->peek((char *)&header, qint64(sizeof(chat_msg_header)));

    if(socket->bytesAvailable() < header.originsize + sizeof(chat_msg_header)) {
        qDebug() << "buffer is not enough for a complete package, socket->bytesAvailable() is " << bytesAvailable;
        return;
    }

    socket->read(sizeof(chat_msg_header));
    QByteArray data = socket->read(header.originsize);
    qDebug() << "Received data:" << data;
    emit dataReceived(data);
}

void TcpClient::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << socketError;
    emit errorOccurred(socketError);
}
