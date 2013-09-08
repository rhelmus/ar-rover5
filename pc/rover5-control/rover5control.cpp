#include "../../shared/tcputils.h"
#include "mapscene.h"
#include "numstatwidget.h"
#include "rover5control.h"
#include "scaledpixmapwidget.h"
#include "tcpclientinterface.h"
#include "utils.h"

#include <QtGui>
#include <QTcpServer>
#include <QTcpSocket>

CRover5Control::CRover5Control(QWidget *parent)
    : QMainWindow(parent), camClientSocket(0), camTcpReadBlockSize(0)
{
    resize(800, 600);

    QWidget *cw = new QWidget(this);
    setCentralWidget(cw);

    QToolBar *toolb = addToolBar("toolbar");
    toolb->addAction(QIcon(":/resources/connect.png"), "Toggle BT", this,
                     SLOT(toggleTcpClientConnection()));
    toolb->addSeparator();
    toggleFrontLEDsAction = toolb->addAction(QIcon(":/resources/light.png"),
                                             "Toggle front LEDs", this,
                                             SLOT(toggleFrontLEDs()));
//    toggleFrontLEDsAction->setCheckable(true);
//    toggleFrontLEDsAction->setChecked(false);
    toggleFrontLEDsAction->setEnabled(false);

    QGridLayout *grid = new QGridLayout(cw);

    grid->addWidget(createStatusWidgets(), 0, 0, 2, 1);
    grid->addWidget(createViewWidgets(), 0, 1);
    grid->addWidget(createDriveWidgets(), 1, 1);

    statusBar()->addPermanentWidget(tcpClientConnectedStatLabel = new QLabel("TCP disconnected"));
    tcpClientConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    statusBar()->addPermanentWidget(camConnectedStatLabel = new QLabel("cam disconnected"));
    camConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);

    initCamServer();

    tcpClientInterface = new CTcpClientInterface(this);
    connect(tcpClientInterface, SIGNAL(connected()), SLOT(tcpClientConnected()));
    connect(tcpClientInterface, SIGNAL(disconnected()), SLOT(tcpClientDisconnected()));
    connect(tcpClientInterface, SIGNAL(msgReceived(EMessage, QByteArray)),
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

    vbox->addWidget(fgroup = createFrameGroupWidget("Odometry", true), 0, Qt::AlignTop);
    fgroup->layout()->addWidget(odoPosXStatW = new CNumStatWidget("X position", 1));
    fgroup->layout()->addWidget(odoPosYStatW = new CNumStatWidget("Y position", 1));
    fgroup->layout()->addWidget(odoRotStatW = new CNumStatWidget("Rotation", 1));

    vbox->addWidget(fgroup = createFrameGroupWidget("Misc.", true), 0, Qt::AlignTop);
    fgroup->layout()->addWidget(batteryStatW = new CNumStatWidget("Battery", 1));
    fgroup->layout()->addWidget(servoPosStatW = new CNumStatWidget("Servo pos", 1));
    fgroup->layout()->addWidget(pingStatW = new CNumStatWidget("Ping", 1));

    vbox->addStretch();

    return ret;
}

QWidget *CRover5Control::createViewWidgets()
{
    QTabWidget *ret = new QTabWidget;

    ret->addTab(createCameraTab(), "Camera");
    ret->addTab(createMapTab(), "Map");
    ret->setCurrentIndex(1);

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

QWidget *CRover5Control::createCameraTab()
{
    QWidget *ret = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout(ret);

    hbox->addWidget(camWidget = new CScaledPixmapWidget);
    camWidget->setRotation(270);

    QFrame *frame = new QFrame;
    frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QFormLayout *form = new QFormLayout(frame);
    hbox->addWidget(frame, 0, Qt::AlignTop);

    form->addRow("Zoom", camZoomSlider = new QSlider(Qt::Horizontal));
    camZoomSlider->setRange(10, 40);
    camZoomSlider->setTickInterval(1);

    form->addRow("Brightness", camBrightnessSpinBox = new QSpinBox);
    camBrightnessSpinBox->setRange(0, 255);

    zoomApplyTimer = new QTimer(this);
    zoomApplyTimer->setInterval(500);
    zoomApplyTimer->setSingleShot(true);
    connect(zoomApplyTimer, SIGNAL(timeout()), SLOT(applyCamZoom()));
    connect(camZoomSlider, SIGNAL(valueChanged(int)), zoomApplyTimer, SLOT(start()));

    return ret;
}

QWidget *CRover5Control::createMapTab()
{
    QWidget *ret = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(ret);

    QGraphicsView *view = new QGraphicsView(mapScene = new CMapScene(this));
    vbox->addWidget(view);

//    mapScene->addPosition(-76.828, -41.255);

    return ret;
}

void CRover5Control::initCamServer()
{
    camServer = new QTcpServer(this);
    if (!camServer->listen(QHostAddress::Any, 40000))
    {
        QMessageBox::critical(this, tr("Rover5 control"),
                              tr("Unable to start cam server: %1.")
                              .arg(camServer->errorString()));
        close();
    }
    else
    {
        connect(camServer, SIGNAL(newConnection()), this, SLOT(handleCamClientConnect()));

        camTcpDisconnectMapper = new QSignalMapper(this);
        connect(camTcpDisconnectMapper, SIGNAL(mapped(QObject *)), this,
                SLOT(handleCamClientDisconnect(QObject *)));
    }
}

bool CRover5Control::canSendCamTcp() const
{
    return (camClientSocket && (camClientSocket->state() == QTcpSocket::ConnectedState));
}

void CRover5Control::parseCamTcp(QDataStream &stream)
{
    uint8_t tmp;
    stream >> tmp;

    const EMessage msg = static_cast<EMessage>(tmp);

    if (msg == MSG_CAMFRAME)
    {
        QByteArray data;
        stream >> data;
        QImage img = QImage::fromData(data, "jpg");
        if (camBrightnessSpinBox->value() > 0)
            changeBrightness(img, camBrightnessSpinBox->value());
        camWidget->setPixmap(QPixmap::fromImage(img));
//        qDebug() << "cam frame size:" << img.size();
    }

//    qDebug() << QString("Received msg: %1 (%2 bytes)\n").arg(msg).arg(tcpReadBlockSize);
}

void CRover5Control::handleCamClientConnect()
{
    qDebug("Client connected\n");

    camConnectedStatLabel->setText("cam connected");
    camConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    if (camClientSocket)
        camClientSocket->disconnectFromHost();

    camClientSocket = camServer->nextPendingConnection();
    connect(camClientSocket, SIGNAL(disconnected()), camTcpDisconnectMapper,
            SLOT(map()));
    connect(camClientSocket, SIGNAL(readyRead()), this, SLOT(camClientHasData()));
}

void CRover5Control::handleCamClientDisconnect(QObject *obj)
{
    // Current client?
    if (camClientSocket == obj)
    {
        camClientSocket = 0;
        camConnectedStatLabel->setText("cam disconnected");
        camConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    }

    obj->deleteLater();
}

void CRover5Control::camClientHasData()
{
//    qDebug() << "clientHasData: " << tcpClientSocket->bytesAvailable() << tcpReadBlockSize;
    QDataStream in(camClientSocket);
    in.setVersion(QDataStream::Qt_4_7);

    while (true)
    {
        if (camTcpReadBlockSize == 0)
        {
            if (camClientSocket->bytesAvailable() < (int)sizeof(uint32_t))
                return;

            in >> camTcpReadBlockSize;
        }

        if (camClientSocket->bytesAvailable() < camTcpReadBlockSize)
            return;

        parseCamTcp(in);
        camTcpReadBlockSize = 0;
    }
}

void CRover5Control::toggleTcpClientConnection()
{
    if (tcpClientInterface->isConnected())
        tcpClientInterface->disconnectFromServer();
    else
    {
        bool ok;
        const QString s = QInputDialog::getText(this, "Server address",
                                                "Server address", QLineEdit::Normal,
                                                "localhost", &ok);
        if (ok && !s.isEmpty())
            tcpClientInterface->connectToServer(s);
    }
}

void CRover5Control::toggleFrontLEDs()
{
    static bool togglev = false; // UNDONE!
    CTcpClientMessage msg(MSG_CMD_FRONTLEDS);
    toggleFrontLEDsAction->toggle(); // UNDONE: Check actual state
//    msg << (uint8_t)toggleFrontLEDsAction->isChecked();
    msg << (uint8_t)(togglev = !togglev);

    tcpClientInterface->send(msg);
}

void CRover5Control::tcpClientConnected()
{
    toggleFrontLEDsAction->setEnabled(true);
    tcpClientConnectedStatLabel->setText("TCP connected");
    tcpClientConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void CRover5Control::tcpClientDisconnected()
{
    toggleFrontLEDsAction->setEnabled(false);
    tcpClientConnectedStatLabel->setText("TCP disconnected");
    tcpClientConnectedStatLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
}

void CRover5Control::btMsgReceived(EMessage m, QByteArray data)
{
//    qDebug() << "Got msg:" << (int)m << "with" << data.size() << "bytes";

    if (m == MSG_MOTOR_TARGETPOWER)
    {
        for (int i=0; i<MOTOR_END; ++i)
            motorPowerStatW[i]->set(0, (uint8_t)data[i]);
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
            motorPowerStatW[i]->set(1, (uint8_t)data[i]);
    }
    else if (m == MSG_ENCODER_SPEED)
    {
        for (int i=0; i<ENC_END; ++i)
            motorSpeedStatW[i]->set(1, (int16_t)bytesToInt(data[i*2], data[i*2+1]));
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
    else if (m == MSG_ODOMETRY)
    {
        const float x = bytesToFloat(data[0], data[1], data[2], data[3]);
        const float y = bytesToFloat(data[4], data[5], data[6], data[7]);
        const float rot = bytesToFloat(data[8], data[9], data[10], data[11]);

        odoPosXStatW->setF(0, x);
        odoPosYStatW->setF(0, y);
        odoRotStatW->setF(0, rot);

        mapScene->addPosition(x, y);
    }
    else if (m == MSG_BATTERY)
        batteryStatW->set(0, bytesToInt(data[0], data[1]));
    else if (m == MSG_SERVO)
        servoPosStatW->set(0, (uint8_t)data[0]);
    else if (m == MSG_IMU)
    {
        pitchStatW->set(0, (int16_t)bytesToInt(data[0], data[1]));
        rollStatW->set(0, (int16_t)bytesToInt(data[2], data[3]));
        headingStatW->set(0, bytesToInt(data[4], data[5]));
    }
    else if (m == MSG_PING)
    {
        tcpClientInterface->send(CTcpClientMessage(MSG_PONG));
        if (!lastPingTime.isNull())
            pingStatW->set(0, lastPingTime.elapsed());
        lastPingTime.restart();
    }
}

void CRover5Control::applyDriveUpdate(CDriveWidget::DriveFlags dir, int drivespeed)
{
    if (dir == CDriveWidget::DRIVE_NONE)
    {
        CTcpClientMessage msg(MSG_CMD_STOP);
        tcpClientInterface->send(msg);
    }
#ifdef MECANUM_MOVEMENT
    else if (dir & CDriveWidget::DRIVE_TRANSLATE)
    {
        ETranslateDirection trdir;
        if (dir & CDriveWidget::DRIVE_FWD)
            trdir = (dir & CDriveWidget::DRIVE_LEFT) ? TRDIR_LEFT_FWD : TRDIR_RIGHT_FWD;
        else if (dir & CDriveWidget::DRIVE_BWD)
            trdir = (dir & CDriveWidget::DRIVE_LEFT) ? TRDIR_LEFT_BWD : TRDIR_RIGHT_BWD;
        else
            trdir = (dir & CDriveWidget::DRIVE_LEFT) ? TRDIR_LEFT : TRDIR_RIGHT;

        CTcpClientMessage msg(MSG_CMD_TRANSLATE);
        msg << (uint8_t)drivespeed << (uint8_t)trdir << (uint16_t)DRIVE_TIME;
        btInterface->send(msg);
    }
#endif
    else if (dir & (CDriveWidget::DRIVE_FWD | CDriveWidget::DRIVE_BWD))
    {
        uint8_t lspeed, rspeed;
        const EMotorDirection mdir = (dir & CDriveWidget::DRIVE_FWD) ? MDIR_FWD : MDIR_BWD;

        if (((mdir == MDIR_FWD) && (dir & CDriveWidget::DRIVE_LEFT)) ||
            ((mdir == MDIR_BWD) && (dir & CDriveWidget::DRIVE_RIGHT)))
        {
            lspeed = 0;
            rspeed = drivespeed;
        }
        else if (((mdir == MDIR_FWD) && (dir & CDriveWidget::DRIVE_RIGHT)) ||
                 ((mdir == MDIR_BWD) && (dir & CDriveWidget::DRIVE_LEFT)))
        {
            lspeed = drivespeed;
            rspeed = 0;
        }
        else // straight fwd/bwd
            lspeed = rspeed = drivespeed;

        CTcpClientMessage msg(MSG_CMD_MOTORSPEED);
        msg << lspeed << rspeed << (uint8_t)mdir << (uint8_t)mdir << (uint16_t)DRIVE_TIME;
        tcpClientInterface->send(msg);
    }
    else // turn
    {
        const ETurnDirection tdir =
                (dir & CDriveWidget::DRIVE_LEFT) ? TDIR_LEFT : TDIR_RIGHT;

        CTcpClientMessage msg(MSG_CMD_TURN);
        msg << (uint8_t)drivespeed << (uint8_t)tdir << (uint16_t)DRIVE_TIME;
        tcpClientInterface->send(msg);
    }
}

void CRover5Control::driveContinuous(int speed, int duration, EMotorDirection dir)
{
    CTcpClientMessage msg(MSG_CMD_MOTORSPEED);
    msg << (uint8_t)speed << (uint8_t)speed << (uint8_t)dir << (uint8_t)dir <<
           (uint16_t)duration;
    tcpClientInterface->send(msg);
}

void CRover5Control::driveDistance(int speed, int dist, EMotorDirection dir)
{
    CTcpClientMessage msg(MSG_CMD_DRIVEDIST);
    msg << (uint8_t)speed << (uint16_t)dist << (uint8_t)dir;
    tcpClientInterface->send(msg);
}

void CRover5Control::turnContinuous(int speed, int duration, ETurnDirection dir)
{
    CTcpClientMessage msg(MSG_CMD_TURN);
    msg << (uint8_t)speed << (uint8_t)dir << (uint16_t)duration;
    tcpClientInterface->send(msg);
}

void CRover5Control::turnAngle(int speed, int angle, ETurnDirection dir)
{
    CTcpClientMessage msg(MSG_CMD_TURNANGLE);
    msg << (uint8_t)speed << (uint16_t)angle << (uint8_t)dir;
    tcpClientInterface->send(msg);
}

void CRover5Control::stopDrive()
{
    tcpClientInterface->send(CTcpClientMessage(MSG_CMD_STOP));
}

void CRover5Control::applyCamZoom()
{
    if (canSendCamTcp())
    {
        CTcpMsgComposer tcpmsg(MSG_SETZOOM);
        tcpmsg << static_cast<qreal>(camZoomSlider->value() / 10.0);
        camClientSocket->write(tcpmsg);
        qDebug() << "send zoom: " << static_cast<qreal>(camZoomSlider->value() / 10.0);
    }
}
