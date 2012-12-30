#include "../../shared/shared.h"
#include "btinterface.h"
#include "rover5control.h"

#include <QtGui>
#include <QTcpServer>
#include <QTcpSocket>

CRover5Control::CRover5Control(QWidget *parent)
    : QMainWindow(parent), tcpClientSocket(0), tcpReadBlockSize(0)
{
    QWidget *cw = new QWidget(this);
    setCentralWidget(cw);

    QVBoxLayout *vbox = new QVBoxLayout(cw);

    vbox->addWidget(camWidget = new QLabel("Cam widget"));

    vbox->addWidget(btConnectButton = new QPushButton("BT connect"));
    connect(btConnectButton, SIGNAL(clicked()), SLOT(toggleBtConnection()));

#if 0
    QPushButton *bttestbutton = new QPushButton("BT send test");
    connect(bttestbutton, SIGNAL(clicked()), SLOT(btSendTest()));
    vbox->addWidget(bttestbutton);
#endif

    initTcpServer();

    btInterface = new CBTInterface(this);
    connect(btInterface, SIGNAL(connected()), SLOT(btConnected()));
    connect(btInterface, SIGNAL(disconnected()), SLOT(btDisconnected()));
}

CRover5Control::~CRover5Control()
{
    
}

void CRover5Control::initTcpServer()
{
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, 40000))
    {
        QMessageBox::critical(this, tr("Rover5 control"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
    }
    else
    {
        connect(tcpServer, SIGNAL(newConnection()), this, SLOT(tcpClientConnected()));

        tcpDisconnectMapper = new QSignalMapper(this);
        connect(tcpDisconnectMapper, SIGNAL(mapped(QObject *)), this,
                SLOT(tcpClientDisconnected(QObject *)));
    }
}

bool CRover5Control::canSendTcp() const
{
    return (tcpClientSocket && (tcpClientSocket->state() == QTcpSocket::ConnectedState));
}

void CRover5Control::parseTcp(QDataStream &stream)
{
    uint8_t tmp;
    stream >> tmp;

    const EMessage msg = static_cast<EMessage>(tmp);

    if (msg == MSG_CAMFRAME)
    {
        QByteArray data;
        stream >> data;
        camWidget->setPixmap(QPixmap::fromImage(QImage::fromData(data, "jpg")));
    }

    qDebug() << QString("Received msg: %1 (%2 bytes)\n").arg(msg).arg(tcpReadBlockSize);
}

void CRover5Control::tcpClientConnected()
{
    qDebug("Client connected\n");

    if (tcpClientSocket)
        tcpClientSocket->disconnectFromHost();

    tcpClientSocket = tcpServer->nextPendingConnection();
    connect(tcpClientSocket, SIGNAL(disconnected()), tcpDisconnectMapper,
            SLOT(map()));
    connect(tcpClientSocket, SIGNAL(readyRead()), this, SLOT(tcpClientHasData()));
}

void CRover5Control::tcpClientDisconnected(QObject *obj)
{
    // Current client?
    if (tcpClientSocket == obj)
        tcpClientSocket = 0;

    obj->deleteLater();
}

void CRover5Control::tcpClientHasData()
{
    qDebug() << "clientHasData: " << tcpClientSocket->bytesAvailable() << tcpReadBlockSize;
    QDataStream in(tcpClientSocket);
    in.setVersion(QDataStream::Qt_4_7);

    while (true)
    {
        if (tcpReadBlockSize == 0)
        {
            if (tcpClientSocket->bytesAvailable() < (int)sizeof(quint32))
                return;

            in >> tcpReadBlockSize;
        }

        if (tcpClientSocket->bytesAvailable() < tcpReadBlockSize)
            return;

        parseTcp(in);
        tcpReadBlockSize = 0;
    }
}

void CRover5Control::toggleBtConnection()
{
    if (btInterface->isConnected())
        btInterface->disconnectBT();
    else
        btInterface->connectBT();
}

void CRover5Control::btConnected()
{
    btConnectButton->setText("BT disconnect");
}

void CRover5Control::btDisconnected()
{
    btConnectButton->setText("BT connect");
}
