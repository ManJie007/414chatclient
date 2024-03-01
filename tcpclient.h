#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <memory>
#include "Msg.h"

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

    void connectToServer(const QString &address, quint16 port);
    bool isConnected() const;
    void sendData(const QByteArray &data);

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &data);
    void errorOccurred(QAbstractSocket::SocketError socketError);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    std::unique_ptr<QTcpSocket> socket;
};

#endif // TCPCLIENT_H
