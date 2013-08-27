#ifndef SERIALBRIDGE_H
#define SERIALBRIDGE_H

#include <QObject>
#include <QSerialPort>

class CSerialBridge : public QObject
{
    Q_OBJECT

    QSerialPort *serialPort;

private slots:
    void handleSerialError(QSerialPort::SerialPortError);
    void readSerial(void);

public:
    CSerialBridge(const QString &port, QObject *parent = 0);
};

#endif // SERIALBRIDGE_H
