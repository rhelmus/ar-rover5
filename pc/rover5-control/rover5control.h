#ifndef ROVER5CONTROL_H
#define ROVER5CONTROL_H

#include "../../shared/shared.h"
#include "drivewidget.h"

#include <stdint.h>

#include <QMainWindow>

class QLabel;
class QPushButton;
class QSignalMapper;
class QTcpServer;
class QTcpSocket;

class CBTInterface;
class CNumStatWidget;

class CRover5Control : public QMainWindow
{
    Q_OBJECT

    QLabel *smallCamWidget, *largeCamWidget;
    QPushButton *btConnectButton;

    CNumStatWidget *motorTargetPowerStatW, *motorTargetSpeedStatW, *motorTargetDistStatW;
    CNumStatWidget *motorSetPowerStatW, *motorSetSpeedStatW, *motorSetDistStatW;
    CNumStatWidget *motorCurrentStatW;
    CNumStatWidget *sharpIRLRStatW, *sharpIRLFRFStatW, *sharpIRFrontStatW;
    CNumStatWidget *sharpIRTurretStatW;
    CNumStatWidget *batteryStatW, *servoPosStatW, *pingStatW;
    CNumStatWidget *headingStatW;

    QTcpServer *tcpServer;
    QSignalMapper *tcpDisconnectMapper;
    QTcpSocket *tcpClientSocket;
    uint32_t tcpReadBlockSize;

    CBTInterface *btInterface;

    QWidget *createTopTabWidget(void);
    QWidget *createGeneralTab(void);
    QWidget *createCamViewTab(void);
    QWidget *createBottomTabWidget(void);
    QWidget *createDriveTab(void);
    QWidget *createCamControlTab(void);
    QLabel *createSmallCamWidget(void);

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
    void setDriveDirection(CDriveWidget::DriveFlags dir);
    
public:
    CRover5Control(QWidget *parent = 0);
    ~CRover5Control(void);
};

#endif // ROVER5CONTROL_H
