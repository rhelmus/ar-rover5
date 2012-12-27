#ifndef ROVER5CONTROL_H
#define ROVER5CONTROL_H

#include <stdint.h>

#include <QMainWindow>

class QDataStream;
class QLabel;
class QSignalMapper;
class QTcpServer;
class QTcpSocket;

class CRover5Control : public QMainWindow
{
    Q_OBJECT

    QLabel *camWidget;
    QTcpServer *tcpServer;
    QSignalMapper *disconnectMapper;
    QTcpSocket *clientSocket;
    uint32_t tcpReadBlockSize;

    void initTcpServer(void);
    bool canSendTcp(void) const;
    void parseTcp(QDataStream &stream);
    void showFrame(QImage data);

private slots:
    void clientConnected(void);
    void clientDisconnected(QObject *obj);
    void clientHasData(void);
    
public:
    CRover5Control(QWidget *parent = 0);
    ~CRover5Control(void);
};

#endif // ROVER5CONTROL_H
