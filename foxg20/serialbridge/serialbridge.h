#ifndef SERIALBRIDGE_H
#define SERIALBRIDGE_H

#include <QObject>
#include <QSerialPort>

class CTcpServer;

class CSerialBridge : public QObject
{
    Q_OBJECT

    QSerialPort *serialPort;
    CTcpServer *tcpServer;

private slots:
    void handleSerialError(QSerialPort::SerialPortError);
    void readSerial(void);
    void readTcp(const QByteArray &ba);

public:
    CSerialBridge(const QString &port, QObject *parent = 0);
};

#endif // SERIALBRIDGE_H
