#include "tcpserver.h"

#include <QDebug>
#include <QtNetwork>

CTcpServer::CTcpServer(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, 40001))
    {
        qCritical() << "Failed to start server:" << tcpServer->errorString();
        QCoreApplication::exit(1);
        return;
    }

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(clientConnected()));

    disconnectMapper = new QSignalMapper(this);
    connect(disconnectMapper, SIGNAL(mapped(QObject *)), this,
            SLOT(clientDisconnected(QObject *)));

    clientDataMapper = new QSignalMapper(this);
    connect(clientDataMapper, SIGNAL(mapped(QObject *)), this,
            SLOT(clientHasData(QObject *)));
}

void CTcpServer::clientConnected()
{
    qDebug() << "Client connected\n";

    QTcpSocket *socket = tcpServer->nextPendingConnection();

    connect(socket, SIGNAL(disconnected()), disconnectMapper, SLOT(map()));
    disconnectMapper->setMapping(socket, socket);

    connect(socket, SIGNAL(readyRead()), clientDataMapper, SLOT(map()));
    clientDataMapper->setMapping(socket, socket);

    clientInfo[socket] = 0;
}

void CTcpServer::clientDisconnected(QObject *obj)
{
    clientInfo.remove(qobject_cast<QTcpSocket *>(obj));
    qDebug() << "Client disconnected";
    obj->deleteLater();
}

void CTcpServer::clientHasData(QObject *obj)
{
    qDebug() << "Received client data\n";

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(obj);
    emit dataReceived(socket->readAll());

#if 0
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_4);

    while (true)
    {
        if (clientInfo[socket] == 0)
        {
            if (socket->bytesAvailable() < (int)sizeof(quint32))
                return;

            in >> clientInfo[socket];
        }

        if (socket->bytesAvailable() < clientInfo[socket])
            return;

        clientTcpReceived(in);
        clientInfo[socket] = 0;
    }
#endif
}

void CTcpServer::send(const QByteArray &by)
{
    for (QMap<QTcpSocket *, quint32>::iterator it=clientInfo.begin();
        it!=clientInfo.end(); ++it)
    {
        it.key()->write(by);
    }
}
