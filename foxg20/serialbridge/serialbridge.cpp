#include "serialbridge.h"
#include "tcpserver.h"

#include <QDebug>
#include <QSerialPortInfo>

CSerialBridge::CSerialBridge(const QString &port, QObject *parent) :
    QObject(parent)
{
    serialPort = new QSerialPort(this);

    serialPort->setPortName(port);

    if (!serialPort->open(QIODevice::ReadWrite) || !serialPort->setBaudRate(QSerialPort::Baud115200) ||
        !serialPort->setDataBits(QSerialPort::Data8) || !serialPort->setParity(QSerialPort::NoParity) ||
        !serialPort->setStopBits(QSerialPort::OneStop) ||
        !serialPort->setFlowControl(QSerialPort::NoFlowControl))
    {
        qDebug() << "Error...: " << serialPort->errorString();
        qFatal("Bailing out...");
    }

    // NOTE: connect error handler after opening port, otherwise it will be called for no good reason(?)
    connect(serialPort, SIGNAL(error(QSerialPort::SerialPortError)),
            SLOT(handleSerialError(QSerialPort::SerialPortError)));
    connect(serialPort, SIGNAL(readyRead()), SLOT(readSerial()));

    tcpServer = new CTcpServer(this);
    connect(tcpServer, SIGNAL(dataReceived(const QByteArray &)), SLOT(readTcp(const QByteArray &)));
}

void CSerialBridge::handleSerialError(QSerialPort::SerialPortError)
{
    qDebug() << "ERROR: " << serialPort->errorString();
    qFatal("Bailing out...");
}

void CSerialBridge::readSerial()
{
    tcpServer->send(serialPort->readAll());
}

void CSerialBridge::readTcp(const QByteArray &ba)
{
    serialPort->write(ba);
}
