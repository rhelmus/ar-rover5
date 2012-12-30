#include "btinterface.h"
#include "numstatwidget.h"
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

    vbox->addWidget(createTopTabWidget());

    QWidget *bw = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout(bw);
    vbox->addWidget(bw);

    hbox->addWidget(createBottomTabWidget());
    hbox->addWidget(smallCamWidget = createSmallCamWidget());

    vbox->addWidget(btConnectButton = new QPushButton("BT connect"));
    connect(btConnectButton, SIGNAL(clicked()), SLOT(toggleBtConnection()));

    initTcpServer();

    btInterface = new CBTInterface(this);
    connect(btInterface, SIGNAL(connected()), SLOT(btConnected()));
    connect(btInterface, SIGNAL(disconnected()), SLOT(btDisconnected()));
    connect(btInterface, SIGNAL(msgReceived(EMessage, QByteArray)),
            SLOT(btMsgReceived(EMessage, QByteArray)));
}

CRover5Control::~CRover5Control()
{
    
}

QWidget *CRover5Control::createTopTabWidget()
{
    QTabWidget *ret = new QTabWidget;
    ret->addTab(createGeneralTab(), "General");
    ret->addTab(createCamViewTab(), "Camera");
    return ret;
}

QWidget *CRover5Control::createGeneralTab()
{
    QWidget *ret = new QWidget;

    QGridLayout *grid = new QGridLayout(ret);

    QGroupBox *group = new QGroupBox("Motor - target");
    grid->addWidget(group, 0, 0);
    QVBoxLayout *svbox = new QVBoxLayout(group);

    svbox->addWidget(motorTargetPowerStatW = new CNumStatWidget("Power", 4));
    svbox->addWidget(motorTargetSpeedStatW = new CNumStatWidget("Speed", 4));
    svbox->addWidget(motorTargetDistStatW = new CNumStatWidget("Dist", 4));

    QLabel *l = new QLabel("(LF/LB/RF/RB)");
    l->setAlignment(Qt::AlignHCenter);
    svbox->addWidget(l, 0, Qt::AlignBottom);


    group = new QGroupBox("Motor - actual");
    grid->addWidget(group, 1, 0);
    svbox = new QVBoxLayout(group);

    svbox->addWidget(motorSetPowerStatW = new CNumStatWidget("Power", 4));
    svbox->addWidget(motorSetSpeedStatW = new CNumStatWidget("Speed", 4));
    svbox->addWidget(motorSetDistStatW = new CNumStatWidget("Dist", 4));
    svbox->addWidget(motorCurrentStatW = new CNumStatWidget("Current", 4));

    svbox->addWidget(l = new QLabel("(LF/LB/RF/RB)"), 0, Qt::AlignBottom);
    l->setAlignment(Qt::AlignHCenter);


    group = new QGroupBox("Sharp IR");
    grid->addWidget(group, 0, 1);
    svbox = new QVBoxLayout(group);

    svbox->addWidget(sharpIRLRStatW = new CNumStatWidget("L / R", 2));
    svbox->addWidget(sharpIRLFRFStatW = new CNumStatWidget("LF / RF", 2));
    svbox->addWidget(sharpIRFrontStatW = new CNumStatWidget("Front", 1));
    svbox->addWidget(sharpIRTurretStatW = new CNumStatWidget("Turret", 1));


    group = new QGroupBox("Misc.");
    grid->addWidget(group, 0, 2);
    svbox = new QVBoxLayout(group);

    svbox->addWidget(batteryStatW = new CNumStatWidget("Battery", 1));
    svbox->addWidget(servoPosStatW = new CNumStatWidget("Servo pos", 1));
    svbox->addWidget(pingStatW = new CNumStatWidget("Ping", 1));


    group = new QGroupBox("IMU");
    grid->addWidget(group, 1, 1);
    svbox = new QVBoxLayout(group);

    svbox->addWidget(headingStatW = new CNumStatWidget("Heading", 1));
    svbox->addWidget(new CNumStatWidget("Pitch", 1));
    svbox->addWidget(new CNumStatWidget("Yaw", 1));


    return ret;
}

QWidget *CRover5Control::createCamViewTab()
{
    QWidget *ret = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(ret);

    vbox->addWidget(largeCamWidget = new QLabel("Cam widget"));
    largeCamWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    largeCamWidget->setMinimumWidth(360);
    largeCamWidget->setMinimumHeight(480);

    return ret;
}

QWidget *CRover5Control::createBottomTabWidget()
{
    QTabWidget *ret = new QTabWidget;
    ret->addTab(createDriveTab(), "Drive");
    ret->addTab(createCamControlTab(), "Cam control");
    return ret;
}

QWidget *CRover5Control::createDriveTab()
{
    return new QWidget;
}

QWidget *CRover5Control::createCamControlTab()
{
    return new QWidget;
}

QLabel *CRover5Control::createSmallCamWidget()
{
    QLabel *ret = new QLabel("Cam widget");
    ret->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ret->setMinimumWidth(200);
    ret->setMinimumHeight(150);
    return ret;
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
        QImage img = QImage::fromData(data, "jpg");
        QMatrix m;
        m.rotate(270);
        img = img.transformed(m);
        smallCamWidget->setPixmap(QPixmap::fromImage(img.scaled(smallCamWidget->size(),
                                                                Qt::IgnoreAspectRatio,
                                                                Qt::SmoothTransformation)));
        largeCamWidget->setPixmap(QPixmap::fromImage(img.scaled(largeCamWidget->size(),
                                                                Qt::IgnoreAspectRatio,
                                                                Qt::SmoothTransformation)));
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

void CRover5Control::btMsgReceived(EMessage m, QByteArray data)
{
    qDebug() << "Got msg:" << (int)m << "with" << data.size() << "bytes";

    if (m == MSG_MOTOR_TARGETPOWER)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorTargetPowerStatW->set(i, data[i]);
    }
    else if (m == MSG_MOTOR_TARGETSPEED)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorTargetSpeedStatW->set(i, bytesToInt(data[i*2], data[i*2+1]));
    }
    else if (m == MSG_MOTOR_TARGETDIST)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorTargetDistStatW->set(i, bytesToInt(data[i*2], data[i*2+1]));
    }
    else if (m == MSG_MOTOR_SETPOWER)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorSetPowerStatW->set(i, data[i]);
    }
    else if (m == MSG_ENCODER_SPEED)
    {
        for (int i=0; i<ENC_END; ++i)
            motorSetPowerStatW->set(i, bytesToInt(data[i*2], data[i*2+1]));
    }
    else if (m == MSG_ENCODER_DISTANCE)
    {
        for (int i=0; i<ENC_END; ++i)
        {
            motorSetPowerStatW->set(i, bytesToLong(data[i*4], data[i*4+1], data[i*4+2],
                    data[i*4+3]));
        }
    }
    else if (m == MSG_MOTOR_CURRENT)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorCurrentStatW->set(i, bytesToInt(data[i*2], data[i*2+1]));
    }
    else if (m == MSG_SHARPIR)
    {
        sharpIRLRStatW->set(0, (uint8_t)data[SHARPIR_LEFT]);
        sharpIRLRStatW->set(1, (uint8_t)data[SHARPIR_RIGHT]);

        sharpIRLFRFStatW->set(0, (uint8_t)data[SHARPIR_LEFT_FW]);
        sharpIRLFRFStatW->set(1, (uint8_t)data[SHARPIR_RIGHT_FW]);

        sharpIRFrontStatW->set(0, (uint8_t)data[SHARPIR_FW]);

        sharpIRTurretStatW->set(0, (uint8_t)data[SHARPIR_TURRET]);
    }
    else if (m == MSG_BATTERY)
        batteryStatW->set(0, bytesToInt(data[0], data[1]));
    else if (m == MSG_SERVO)
        servoPosStatW->set(0, data[0]);
    else if (m == MSG_HEADING)
        headingStatW->set(0, bytesToInt(data[0], data[1]));
}
