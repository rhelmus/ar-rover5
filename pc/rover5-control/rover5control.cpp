#include "rover5control.h"

#include <QtGui>
#include <QTcpServer>
#include <QTcpSocket>

CRover5Control::CRover5Control(QWidget *parent)
    : QMainWindow(parent), clientSocket(0), tcpReadBlockSize(0)
{
    QWidget *cw = new QWidget(this);
    setCentralWidget(cw);

    QVBoxLayout *vbox = new QVBoxLayout(cw);

    vbox->addWidget(camWidget = new QLabel("Cam widget"));

    initTcpServer();
}

CRover5Control::~CRover5Control()
{
    
}

void CRover5Control::initTcpServer()
{
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, 40000))
    {
        QMessageBox::critical(this, tr("Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
    }
    else
    {
        connect(tcpServer, SIGNAL(newConnection()), this, SLOT(clientConnected()));

        disconnectMapper = new QSignalMapper(this);
        connect(disconnectMapper, SIGNAL(mapped(QObject *)), this,
                SLOT(clientDisconnected(QObject *)));

    }
}

bool CRover5Control::canSendTcp() const
{
    return (clientSocket && (clientSocket->state() == QTcpSocket::ConnectedState));
}

void CRover5Control::parseTcp(QDataStream &stream)
{
    uint8_t msg;
    stream >> msg;

    if (msg == 0)
    {
        QByteArray data;
        stream >> data;
        camWidget->setPixmap(QPixmap::fromImage(QImage::fromData(data, "jpg")));
    }

    qDebug() << QString("Received msg: %1 (%2 bytes)\n").arg(msg).arg(tcpReadBlockSize);
}

void CRover5Control::clientConnected()
{
    qDebug("Client connected\n");

    if (clientSocket)
        clientSocket->disconnectFromHost();

    clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()), disconnectMapper,
            SLOT(map()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(clientHasData()));
}

void CRover5Control::clientDisconnected(QObject *obj)
{
    // Current client?
    if (clientSocket == obj)
        clientSocket = 0;

    obj->deleteLater();
}

void CRover5Control::clientHasData()
{
    qDebug() << "clientHasData: " << clientSocket->bytesAvailable() << tcpReadBlockSize;
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_4_7);

    while (true)
    {
        if (tcpReadBlockSize == 0)
        {
            if (clientSocket->bytesAvailable() < (int)sizeof(quint32))
                return;

            in >> tcpReadBlockSize;
        }

        if (clientSocket->bytesAvailable() < tcpReadBlockSize)
            return;

        parseTcp(in);
        tcpReadBlockSize = 0;
    }
}
