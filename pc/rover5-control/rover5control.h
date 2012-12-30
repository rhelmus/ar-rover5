#ifndef ROVER5CONTROL_H
#define ROVER5CONTROL_H

#include <stdint.h>

#include <QMainWindow>

class QLabel;
class QPushButton;
class QSignalMapper;
class QTcpServer;
class QTcpSocket;

class CBTInterface;

class CRover5Control : public QMainWindow
{
    Q_OBJECT

    QLabel *camWidget;
    QPushButton *btConnectButton;

    QTcpServer *tcpServer;
    QSignalMapper *tcpDisconnectMapper;
    QTcpSocket *tcpClientSocket;
    uint32_t tcpReadBlockSize;

    CBTInterface *btInterface;

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
    
public:
    CRover5Control(QWidget *parent = 0);
    ~CRover5Control(void);
};

#endif // ROVER5CONTROL_H
