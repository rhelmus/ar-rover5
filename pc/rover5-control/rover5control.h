#ifndef ROVER5CONTROL_H
#define ROVER5CONTROL_H

#include "../../shared/shared.h"
#include "drivewidget.h"

#include <stdint.h>

#include <QMainWindow>
#include <QTime>

class QLabel;
class QPushButton;
class QSignalMapper;
class QSlider;
class QTcpServer;
class QTcpSocket;

class CBTInterface;
class CNumStatWidget;
class CScaledPixmapWidget;

class CRover5Control : public QMainWindow
{
    Q_OBJECT

    enum { DRIVE_TIME = 1 };

    QLabel *btConnectedStatLabel, *tcpConnectedStatLabel;

    CScaledPixmapWidget *camWidget;

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
    QTime lastPingTime;

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
    void applyDriveUpdate(CDriveWidget::DriveFlags dir, int drivespeed);
    void driveContinuous(int speed, int duration, EMotorDirection dir);
    void driveDistance(int speed, int dist, EMotorDirection dir);
    void turnContinuous(int speed, int duration, ETurnDirection dir);
    void turnAngle(int speed, int angle, ETurnDirection dir);
    void stopDrive(void);
    void applyCamZoom(void);

protected:
    virtual void closeEvent(QCloseEvent *event);
    
public:
    CRover5Control(QWidget *parent = 0);
    ~CRover5Control(void);
};

#endif // ROVER5CONTROL_H
