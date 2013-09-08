#ifndef TCPCLIENTINTERFACE_H
#define TCPCLIENTINTERFACE_H

#include "../../shared/shared.h"

#include <QObject>
#include <QTcpSocket>

class CTcpClientInterface : public QObject
{
    Q_OBJECT

    QTcpSocket *tcpSocket;
    QByteArray recBuffer;

private slots:
    void handleDisconnect(void);
    void readTcpData(void);
    void handleSocketError(QAbstractSocket::SocketError);

public:
    explicit CTcpClientInterface(QObject *parent = 0);

    void connectToServer(const QString &s);
    void disconnectFromServer(void) { tcpSocket->abort(); }
    bool isConnected(void) const
    { return (tcpSocket->state() == QAbstractSocket::ConnectedState); }
    void send(const QByteArray &data);

signals:
    void connected(void);
    void disconnected(void);
    void msgReceived(EMessage, QByteArray);
    void tcpError(const QString &);
};

class CTcpClientMessage
{
    QByteArray data;

public:
    CTcpClientMessage(EMessage m);
    CTcpClientMessage(void);

    void setMessage(EMessage m) { data[2] = static_cast<char>(m); }
    CTcpClientMessage &operator <<(uint8_t b) { data.push_back(b); return *this; }
    CTcpClientMessage &operator <<(uint16_t i);
    operator QByteArray(void);
};

#endif // TCPCLIENTINTERFACE_H
