#include "../../shared/tcputils.h"
#include "btinterface.h"
#include "numstatwidget.h"
#include "rover5control.h"
#include "scaledpixmapwidget.h"
#include "utils.h"

#include <QtGui>
#include <QTcpServer>
#include <QTcpSocket>

CRover5Control::CRover5Control(QWidget *parent)
    : QMainWindow(parent), tcpClientSocket(0), tcpReadBlockSize(0)
{
    QWidget *cw = new QWidget(this);
    setCentralWidget(cw);

#if 0
    QVBoxLayout *vbox = new QVBoxLayout(cw);

    vbox->addWidget(createTopTabWidget());

    QWidget *bw = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout(bw);
    vbox->addWidget(bw);

    hbox->addWidget(createBottomTabWidget());
    hbox->addWidget(smallCamWidget = createSmallCamWidget());

    vbox->addWidget(btConnectButton = new QPushButton("BT connect"));
    connect(btConnectButton, SIGNAL(clicked()), SLOT(toggleBtConnection()));

    QPushButton *button = new QPushButton("BT send");
    connect(button, SIGNAL(clicked()), SLOT(btSendTest()));
    vbox->addWidget(button);
#endif

    QToolBar *toolb = addToolBar("toolbar");
    toolb->addAction(QIcon(":/resources/connect.png"), "Toggle BT", this,
                     SLOT(toggleBtConnection()));

    QGridLayout *grid = new QGridLayout(cw);

    grid->addWidget(createStatusWidgets(), 0, 0, 2, 1);
    grid->addWidget(createCameraWidgets(), 0, 1);
    grid->addWidget(createDriveWidgets(), 1, 1);

    statusBar()->addPermanentWidget(btConnectedStatLabel = new QLabel("BT disconnected"));
    btConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    statusBar()->addPermanentWidget(tcpConnectedStatLabel = new QLabel("TCP disconnected"));
    tcpConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);

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

QWidget *CRover5Control::createStatusWidgets()
{
    QTabWidget *ret = new QTabWidget;
    ret->setTabPosition(QTabWidget::South);
    ret->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ret->setMinimumWidth(220);

    QWidget *w = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(w);
    vbox->setSpacing(0);
    vbox->setSpacing(0);
    ret->addTab(w, "Motors");

    QFrame *fgroup = createFrameGroupWidget("Power", true);
    vbox->addWidget(fgroup, 0, Qt::AlignTop);
    fgroup->layout()->addWidget(motorPowerStatW[MOTOR_LF] = new CNumStatWidget("Left FWD", 2));
    fgroup->layout()->addWidget(motorPowerStatW[MOTOR_LB] = new CNumStatWidget("Left BWD", 2));
    fgroup->layout()->addWidget(motorPowerStatW[MOTOR_RF] = new CNumStatWidget("Right FWD", 2));
    fgroup->layout()->addWidget(motorPowerStatW[MOTOR_RB] = new CNumStatWidget("Right BWD", 2));

    vbox->addWidget(fgroup = createFrameGroupWidget("Speed", true), 0, Qt::AlignTop);
    fgroup->layout()->addWidget(motorSpeedStatW[MOTOR_LF] = new CNumStatWidget("Left FWD", 2));
    fgroup->layout()->addWidget(motorSpeedStatW[MOTOR_LB] = new CNumStatWidget("Left BWD", 2));
    fgroup->layout()->addWidget(motorSpeedStatW[MOTOR_RF] = new CNumStatWidget("Right FWD", 2));
    fgroup->layout()->addWidget(motorSpeedStatW[MOTOR_RB] = new CNumStatWidget("Right BWD", 2));

    vbox->addWidget(fgroup = createFrameGroupWidget("Distance", true), 0, Qt::AlignTop);
    fgroup->layout()->addWidget(motorDistStatW[MOTOR_LF] = new CNumStatWidget("Left FWD", 2));
    fgroup->layout()->addWidget(motorDistStatW[MOTOR_LB] = new CNumStatWidget("Left BWD", 2));
    fgroup->layout()->addWidget(motorDistStatW[MOTOR_RF] = new CNumStatWidget("Right FWD", 2));
    fgroup->layout()->addWidget(motorDistStatW[MOTOR_RB] = new CNumStatWidget("Right BWD", 2));

    vbox->addWidget(fgroup = createFrameGroupWidget("Current", true), 0, Qt::AlignTop);
    fgroup->layout()->addWidget(motorCurrentStatW[MOTOR_LF] = new CNumStatWidget("Left FWD", 1));
    fgroup->layout()->addWidget(motorCurrentStatW[MOTOR_LB] = new CNumStatWidget("Left BWD", 1));
    fgroup->layout()->addWidget(motorCurrentStatW[MOTOR_RF] = new CNumStatWidget("Right FWD", 1));
    fgroup->layout()->addWidget(motorCurrentStatW[MOTOR_RB] = new CNumStatWidget("Right BWD", 1));

    vbox->addStretch();


    ret->addTab(w = new QWidget, "Others");
    vbox = new QVBoxLayout(w);
    vbox->setSpacing(0);
    vbox->setMargin(0);

    vbox->addWidget(fgroup = createFrameGroupWidget("Sharp IR", true), 0, Qt::AlignTop);
    fgroup->layout()->addWidget(sharpIRLRStatW = new CNumStatWidget("L / R", 2));
    fgroup->layout()->addWidget(sharpIRLFRFStatW = new CNumStatWidget("LF / RF", 2));
    fgroup->layout()->addWidget(sharpIRFrontStatW = new CNumStatWidget("Front", 1));
    fgroup->layout()->addWidget(sharpIRTurretStatW = new CNumStatWidget("Turret", 1));

    vbox->addWidget(fgroup = createFrameGroupWidget("IMU", true), 0, Qt::AlignTop);
    fgroup->layout()->addWidget(pitchStatW = new CNumStatWidget("Pitch", 1));
    fgroup->layout()->addWidget(rollStatW = new CNumStatWidget("Roll", 1));
    fgroup->layout()->addWidget(headingStatW = new CNumStatWidget("Heading", 1));

    vbox->addWidget(fgroup = createFrameGroupWidget("Misc.", true), 0, Qt::AlignTop);
    fgroup->layout()->addWidget(batteryStatW = new CNumStatWidget("Battery", 1));
    fgroup->layout()->addWidget(servoPosStatW = new CNumStatWidget("Servo pos", 1));
    fgroup->layout()->addWidget(pingStatW = new CNumStatWidget("Ping", 1));

    vbox->addStretch();

    return ret;
}

QWidget *CRover5Control::createCameraWidgets()
{
    QWidget *ret = createFrameGroupWidget("Camera");

    ret->layout()->addWidget(camWidget = new CScaledPixmapWidget);
    camWidget->setRotation(270);
//    camWidget->setMinimumWidth(320);
//    camWidget->setMinimumHeight(260);

    ret->layout()->addWidget(camZoomSlider = new QSlider(Qt::Horizontal));
    camZoomSlider->setRange(10, 40);
    camZoomSlider->setTickInterval(1);

    zoomApplyTimer = new QTimer(this);
    zoomApplyTimer->setInterval(500);
    zoomApplyTimer->setSingleShot(true);
    connect(zoomApplyTimer, SIGNAL(timeout()), SLOT(applyCamZoom()));
    connect(camZoomSlider, SIGNAL(valueChanged(int)), zoomApplyTimer, SLOT(start()));

    return ret;
}

QWidget *CRover5Control::createDriveWidgets()
{
    QWidget *ret = createFrameGroupWidget("Drive");
    ret->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QWidget *w = new QWidget;
    w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ret->layout()->addWidget(w);
    QGridLayout *grid = new QGridLayout(w);

    CDriveWidget *dw = new CDriveWidget;
    connect(dw, SIGNAL(driveUpdate(CDriveWidget::DriveFlags, int)),
            SLOT(applyDriveUpdate(CDriveWidget::DriveFlags, int)));
    connect(dw, SIGNAL(driveContReq(int,int,EMotorDirection)),
            SLOT(driveContinuous(int,int,EMotorDirection)));
    connect(dw, SIGNAL(driveDistReq(int,int,EMotorDirection)),
            SLOT(driveDistance(int,int,EMotorDirection)));
    connect(dw, SIGNAL(turnContReq(int,int,ETurnDirection)),
            SLOT(turnContinuous(int,int,ETurnDirection)));
    connect(dw, SIGNAL(turnAngleReq(int,int,ETurnDirection)),
            SLOT(turnAngle(int,int,ETurnDirection)));
    connect(dw, SIGNAL(stopDriveReq()), SLOT(stopDrive()));
    grid->addWidget(dw, 0, 0);

    return ret;

}

#if 0
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

    svbox->addWidget(IMUStatW = new CNumStatWidget("pitch/roll/yaw", 3));


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
    QWidget *ret = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout(ret);

    CDriveWidget *dw = new CDriveWidget;
    connect(dw, SIGNAL(driveUpdate(CDriveWidget::DriveFlags)),
            SLOT(driveUpdate(CDriveWidget::DriveFlags)));
    hbox->addWidget(dw);

    return ret;
}

QWidget *CRover5Control::createCamControlTab()
{
    QWidget *ret = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(ret);

    vbox->addWidget(camZoomSlider = new QSlider(Qt::Vertical));
    camZoomSlider->setRange(10, 40);
    camZoomSlider->setTickInterval(1);

    zoomApplyTimer = new QTimer(this);
    zoomApplyTimer->setInterval(500);
    zoomApplyTimer->setSingleShot(true);
    connect(zoomApplyTimer, SIGNAL(timeout()), SLOT(applyCamZoom()));
    connect(camZoomSlider, SIGNAL(valueChanged(int)), zoomApplyTimer, SLOT(start()));

    return ret;
}
#endif
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
        camWidget->setPixmap(QPixmap::fromImage(img));
//        qDebug() << "cam frame size:" << img.size();
    }

//    qDebug() << QString("Received msg: %1 (%2 bytes)\n").arg(msg).arg(tcpReadBlockSize);
}

void CRover5Control::tcpClientConnected()
{
    qDebug("Client connected\n");

    tcpConnectedStatLabel->setText("TCP connected");
    tcpConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

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
    {
        tcpClientSocket = 0;
        tcpConnectedStatLabel->setText("TCP disconnected");
        tcpConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    }

    obj->deleteLater();
}

void CRover5Control::tcpClientHasData()
{
//    qDebug() << "clientHasData: " << tcpClientSocket->bytesAvailable() << tcpReadBlockSize;
    QDataStream in(tcpClientSocket);
    in.setVersion(QDataStream::Qt_4_7);

    while (true)
    {
        if (tcpReadBlockSize == 0)
        {
            if (tcpClientSocket->bytesAvailable() < (int)sizeof(uint32_t))
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
    btConnectedStatLabel->setText("BT connected");
    btConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void CRover5Control::btDisconnected()
{
    btConnectedStatLabel->setText("BT disconnected");
    btConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
}

void CRover5Control::btMsgReceived(EMessage m, QByteArray data)
{
//    qDebug() << "Got msg:" << (int)m << "with" << data.size() << "bytes";

    if (m == MSG_MOTOR_TARGETPOWER)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorPowerStatW[i]->set(0, data[i]);
    }
    else if (m == MSG_MOTOR_TARGETSPEED)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorSpeedStatW[i]->set(0, bytesToInt(data[i*2], data[i*2+1]));
    }
    else if (m == MSG_MOTOR_TARGETDIST)
    {
        for (int i=0; i<MOTOR_END; ++i)
        {
            motorDistStatW[i]->set(0, bytesToLong(data[i*4], data[i*4+1], data[i*4+2],
                                   data[i*4+3]));
        }
    }
    else if (m == MSG_MOTOR_SETPOWER)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorPowerStatW[i]->set(1, data[i]);
    }
    else if (m == MSG_ENCODER_SPEED)
    {
        for (int i=0; i<ENC_END; ++i)
            motorSpeedStatW[i]->set(1, bytesToInt(data[i*2], data[i*2+1]));
    }
    else if (m == MSG_ENCODER_DISTANCE)
    {
        for (int i=0; i<ENC_END; ++i)
        {
            motorDistStatW[i]->set(1, bytesToLong(data[i*4], data[i*4+1], data[i*4+2],
                                   data[i*4+3]));
        }
    }
    else if (m == MSG_MOTOR_CURRENT)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorCurrentStatW[i]->set(0, bytesToInt(data[i*2], data[i*2+1]));
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
    else if (m == MSG_IMU)
    {
        pitchStatW->set(0, (int16_t)bytesToInt(data[0], data[1]));
        rollStatW->set(0, (int16_t)bytesToInt(data[2], data[3]));
        headingStatW->set(0, bytesToInt(data[4], data[5]));
    }
    else if (m == MSG_PING)
    {
        btInterface->send(CBTMessage(MSG_PONG));
        if (!lastPingTime.isNull())
            pingStatW->set(0, lastPingTime.elapsed());
        lastPingTime.restart();
    }
}

void CRover5Control::applyDriveUpdate(CDriveWidget::DriveFlags dir, int drivespeed)
{
    if (dir == CDriveWidget::DRIVE_NONE)
    {
        CBTMessage msg(MSG_CMD_STOP);
        btInterface->send(msg);
    }
    else if (dir & (CDriveWidget::DRIVE_FWD | CDriveWidget::DRIVE_BWD))
    {
        uint8_t lspeed, rspeed;
        const EMotorDirection mdir = (dir & CDriveWidget::DRIVE_FWD) ? DIR_FWD : DIR_BWD;

        if (((mdir == DIR_FWD) && (dir & CDriveWidget::DRIVE_LEFT)) ||
            ((mdir == DIR_BWD) && (dir & CDriveWidget::DRIVE_RIGHT)))
        {
            lspeed = 0;
            rspeed = drivespeed;
        }
        else if (((mdir == DIR_FWD) && (dir & CDriveWidget::DRIVE_RIGHT)) ||
                 ((mdir == DIR_BWD) && (dir & CDriveWidget::DRIVE_LEFT)))
        {
            lspeed = drivespeed;
            rspeed = 0;
        }
        else // straight fwd/bwd
            lspeed = rspeed = drivespeed;

        CBTMessage msg(MSG_CMD_MOTORSPEED);
        msg << lspeed << rspeed << (uint8_t)mdir << (uint8_t)mdir << (uint16_t)DRIVE_TIME;
        btInterface->send(msg);
    }
    else // turn
    {
        const ETurnDirection tdir =
                (dir & CDriveWidget::DRIVE_LEFT) ? DIR_LEFT : DIR_RIGHT;

        CBTMessage msg(MSG_CMD_TURN);
        msg << (uint8_t)drivespeed << (uint8_t)tdir << (uint16_t)DRIVE_TIME;
        btInterface->send(msg);
    }
}

void CRover5Control::driveContinuous(int speed, int duration, EMotorDirection dir)
{
    CBTMessage msg(MSG_CMD_MOTORSPEED);
    msg << (uint8_t)speed << (uint8_t)speed << (uint8_t)dir << (uint8_t)dir <<
           (uint16_t)duration;
    btInterface->send(msg);
}

void CRover5Control::driveDistance(int speed, int dist, EMotorDirection dir)
{
    CBTMessage msg(MSG_CMD_DRIVEDIST);
    msg << (uint8_t)speed << (uint16_t)dist << (uint8_t)dir;
    btInterface->send(msg);
}

void CRover5Control::turnContinuous(int speed, int duration, ETurnDirection dir)
{
    CBTMessage msg(MSG_CMD_TURN);
    msg << (uint8_t)speed << (uint8_t)dir << (uint16_t)duration;
    btInterface->send(msg);
}

void CRover5Control::turnAngle(int speed, int angle, ETurnDirection dir)
{
    CBTMessage msg(MSG_CMD_TURNANGLE);
    msg << (uint8_t)speed << (uint16_t)angle << (uint8_t)dir;
    btInterface->send(msg);
}

void CRover5Control::stopDrive()
{
    btInterface->send(CBTMessage(MSG_CMD_STOP));
}

void CRover5Control::applyCamZoom()
{
    if (canSendTcp())
    {
        CTcpMsgComposer tcpmsg(MSG_SETZOOM);
        tcpmsg << static_cast<qreal>(camZoomSlider->value() / 10.0);
        tcpClientSocket->write(tcpmsg);
        qDebug() << "send zoom: " << static_cast<qreal>(camZoomSlider->value() / 10.0);
    }
}

void CRover5Control::closeEvent(QCloseEvent *event)
{
    btInterface->disconnectBT();
    QTime waittime;
    waittime.start();

    while ((waittime.elapsed() < 3000) && btInterface->isConnected())
        qApp->processEvents();

    QMainWindow::closeEvent(event);
}
