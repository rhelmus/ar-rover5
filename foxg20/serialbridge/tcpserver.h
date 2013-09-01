#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMap>
#include <QObject>

class QSignalMapper;
class QTcpSocket;
class QTcpServer;

class CTcpServer : public QObject
{
    Q_OBJECT

    QTcpServer *tcpServer;
    QSignalMapper *disconnectMapper, *clientDataMapper;
    QMap<QTcpSocket *, quint32> clientInfo;

private slots:
    void clientConnected(void);
    void clientDisconnected(QObject *obj);
    void clientHasData(QObject *obj);

public:
    explicit CTcpServer(QObject *parent = 0);

    void send(const QByteArray &by);
    
signals:
    void dataReceived(const QByteArray &);
    
};

#endif // TCPSERVER_H
