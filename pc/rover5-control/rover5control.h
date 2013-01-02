#ifndef ROVER5CONTROL_H
#define ROVER5CONTROL_H

#include "../../shared/shared.h"
#include "drivewidget.h"

#include <stdint.h>

#include <QMainWindow>

class QLabel;
class QPushButton;
class QSignalMapper;
class QSlider;
class QTcpServer;
class QTcpSocket;

class CBTInterface;
class CNumStatWidget;

class CRover5Control : public QMainWindow
{
    Q_OBJECT

    enum { DRIVE_TIME = 1 };

    QLabel *smallCamWidget, *largeCamWidget;
    QPushButton *btConnectButton;

    CNumStatWidget *motorPowerStatW[MOTOR_END], *motorSpeedStatW[MOTOR_END];
    CNumStatWidget *motorDistStatW[MOTOR_END], *motorCurrentStatW[MOTOR_END];
    CNumStatWidget *sharpIRLRStatW, *sharpIRLFRFStatW, *sharpIRFrontStatW;
    CNumStatWidget *sharpIRTurretStatW;
    CNumStatWidget *pitchStatW, *rollStatW, *headingStatW;
    CNumStatWidget *batteryStatW, *servoPosStatW, *pingStatW;

    QSlider *camZoomSlider;
    QTimer *zoomApplyTimer;

    QTcpServer *tcpServer;
    QSignalMapper *tcpDisconnectMapper;
    QTcpSocket *tcpClientSocket;
    uint32_t tcpReadBlockSize;

    CBTInterface *btInterface;

    QWidget *createStatusWidgets(void);
    QWidget *createCameraWidgets(void);
    QWidget *createDriveWidgets(void);

    QWidget *createTopTabWidget(void);
    QWidget *createGeneralTab(void);
    QWidget *createCamViewTab(void);
    QWidget *createBottomTabWidget(void);
    QWidget *createDriveTab(void);
    QWidget *createCamControlTab(void);

    void initTcpServer(void);
    bool canSendTcp(void) const;
    void parseTcp(QDataStream &stream);

private slots:
    void tcpClientConnected(void);
    void tcpClientDisconnected(QObject *obj);
    void tcpClientHasData(void);
    void toggleBtConnection(void);
    void btConnected(void);
    void btDisconnected(void);
    void btMsgReceived(EMessage m, QByteArray data);
    void btSendTest(void);
    void driveUpdate(CDriveWidget::DriveFlags dir, int drivespeed);
    void applyCamZoom(void);
    
public:
    CRover5Control(QWidget *parent = 0);
    ~CRover5Control(void);
};

#endif // ROVER5CONTROL_H
