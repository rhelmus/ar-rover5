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

class CMapScene;
class CNumStatWidget;
class CScaledPixmapWidget;
class CTcpClientInterface;

class CRover5Control : public QMainWindow
{
    Q_OBJECT

    enum { DRIVE_TIME = 1 };

    QAction *toggleFrontLEDsAction;
    QLabel *tcpClientConnectedStatLabel, *camConnectedStatLabel;

    CScaledPixmapWidget *camWidget;

    CNumStatWidget *motorPowerStatW[MOTOR_END], *motorSpeedStatW[MOTOR_END];
    CNumStatWidget *motorDistStatW[MOTOR_END], *motorCurrentStatW[MOTOR_END];
    CNumStatWidget *sharpIRLRStatW, *sharpIRLFRFStatW, *sharpIRFrontStatW;
    CNumStatWidget *sharpIRTurretStatW;
    CNumStatWidget *pitchStatW, *rollStatW, *headingStatW;
    CNumStatWidget *odoPosXStatW, *odoPosYStatW, *odoRotStatW;
    CNumStatWidget *batteryStatW, *servoPosStatW, *pingStatW;

    QSlider *camZoomSlider;
    QTimer *zoomApplyTimer;
    QSpinBox *camBrightnessSpinBox;

    CMapScene *mapScene;

    QTcpServer *camServer;
    QSignalMapper *camTcpDisconnectMapper;
    QTcpSocket *camClientSocket;
    uint32_t camTcpReadBlockSize;

    CTcpClientInterface *tcpClientInterface;
    QTime lastPingTime;

    QWidget *createStatusWidgets(void);
    QWidget *createViewWidgets(void);
    QWidget *createDriveWidgets(void);
    QWidget *createCameraTab(void);
    QWidget *createMapTab(void);

    void initCamServer(void);
    bool canSendCamTcp(void) const;
    void parseCamTcp(QDataStream &stream);

private slots:
    void handleCamClientConnect(void);
    void handleCamClientDisconnect(QObject *obj);
    void camClientHasData(void);
    void toggleTcpClientConnection(void);
    void toggleFrontLEDs(void);
    void tcpClientConnected(void);
    void tcpClientDisconnected(void);
    void btMsgReceived(EMessage m, QByteArray data);
    void applyDriveUpdate(CDriveWidget::DriveFlags dir, int drivespeed);
    void driveContinuous(int speed, int duration, EMotorDirection dir);
    void driveDistance(int speed, int dist, EMotorDirection dir);
    void turnContinuous(int speed, int duration, ETurnDirection dir);
    void turnAngle(int speed, int angle, ETurnDirection dir);
    void stopDrive(void);
    void applyCamZoom(void);

public:
    CRover5Control(QWidget *parent = 0);
    ~CRover5Control(void);
};

#endif // ROVER5CONTROL_H
