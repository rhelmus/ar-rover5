#include "../../shared/shared.h"
#include "rover5control.h"

#include <QBluetoothSocket>
#include <QtGui>
#include <QTcpServer>
#include <QTcpSocket>

CRover5Control::CRover5Control(QWidget *parent)
    : QMainWindow(parent), tcpClientSocket(0), tcpReadBlockSize(0),
      bluetoothSocket(0)
{
    QWidget *cw = new QWidget(this);
    setCentralWidget(cw);

    QVBoxLayout *vbox = new QVBoxLayout(cw);

    vbox->addWidget(camWidget = new QLabel("Cam widget"));

    vbox->addWidget(btConnectButton = new QPushButton("BT connect"));
    connect(btConnectButton, SIGNAL(clicked()), SLOT(toggleBtConnection()));

    QPushButton *bttestbutton = new QPushButton("BT send test");
    connect(bttestbutton, SIGNAL(clicked()), SLOT(btSendTest()));
    vbox->addWidget(bttestbutton);

    initTcpServer();
    initBluetooth();
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

void CRover5Control::initBluetooth()
{
    bluetoothSocket = new QtMobility::QBluetoothSocket(QtMobility::QBluetoothSocket::RfcommSocket,
                                                       this);
    connect(bluetoothSocket, SIGNAL(connected()), SLOT(btConnected()));
    connect(bluetoothSocket, SIGNAL(disconnected()), SLOT(btDisconnected()));
    connect(bluetoothSocket, SIGNAL(readyRead()), SLOT(btHasData()));

    btSendTimer = new QTimer(this);
    btSendTimer->setInterval(BTQUEUE_DELAY);
    connect(btSendTimer, SIGNAL(timeout()), SLOT(btSendFromQueue()));
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
    if (bluetoothSocket->state() == QtMobility::QBluetoothSocket::ConnectedState)
        bluetoothSocket->disconnectFromService();
    else
        bluetoothSocket->connectToService(QtMobility::QBluetoothAddress("00:11:67:AD:CD:BB"),
                                          QtMobility::QBluetoothUuid::Rfcomm);
}

void CRover5Control::btConnected()
{
    qDebug() << "Got Bluetooth connection!";
    btConnectButton->setText("BT disconnect");
    btSendTimer->start();
}

void CRover5Control::btDisconnected()
{
    qDebug() << "Lost bt connection";
    btConnectButton->setText("BT connect");
    btSendTimer->stop();
}

void CRover5Control::btHasData()
{
    qDebug() << "Got some bt data:" << bluetoothSocket->bytesAvailable() << "bytes";
    while (bluetoothSocket->canReadLine())
        qDebug() << bluetoothSocket->readLine();
//    qDebug() << "Data:" << bluetoothSocket->readAll();
}

void CRover5Control::btSendFromQueue()
{
    if (bluetoothSocket->state() != QtMobility::QBluetoothSocket::ConnectedState)
        btSendTimer->stop();
    else if (!btSendQueue.isEmpty())
        bluetoothSocket->write(btSendQueue.dequeue());
}

void CRover5Control::btSendTest()
{
    if (bluetoothSocket->state() == QtMobility::QBluetoothSocket::ConnectedState)
    {
//        bluetoothSocket->write("Howdy hoh!\n");
        btSendQueue.enqueue("Howdy hoh!\n");

        for (int i=0; i<90; ++i)
        {
//            bluetoothSocket->write(QString("i: %1\n").arg(i).toLatin1());
            btSendQueue.enqueue(QString("i: %1\n").arg(i).toLatin1());
        }
    }
}
