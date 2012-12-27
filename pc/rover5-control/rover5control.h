#ifndef ROVER5CONTROL_H
#define ROVER5CONTROL_H

#include <stdint.h>

#include <QMainWindow>
#include <QQueue>

namespace QtMobility {
class QBluetoothSocket;
}

class QLabel;
class QPushButton;
class QSignalMapper;
class QTcpServer;
class QTcpSocket;

class CRover5Control : public QMainWindow
{
    Q_OBJECT

    enum { BTQUEUE_DELAY = 100 };

    QLabel *camWidget;
    QPushButton *btConnectButton;

    QTcpServer *tcpServer;
    QSignalMapper *tcpDisconnectMapper;
    QTcpSocket *tcpClientSocket;
    uint32_t tcpReadBlockSize;

    QtMobility::QBluetoothSocket *bluetoothSocket;
    QQueue<QByteArray> btSendQueue;
    QTimer *btSendTimer;

    void initTcpServer(void);
    bool canSendTcp(void) const;
    void parseTcp(QDataStream &stream);
    void initBluetooth(void);

private slots:
    void tcpClientConnected(void);
    void tcpClientDisconnected(QObject *obj);
    void tcpClientHasData(void);
    void toggleBtConnection(void);
    void btConnected(void);
    void btDisconnected(void);
    void btHasData(void);
    void btSendFromQueue(void);
    void btSendTest(void);
    
public:
    CRover5Control(QWidget *parent = 0);
    ~CRover5Control(void);
};

#endif // ROVER5CONTROL_H
