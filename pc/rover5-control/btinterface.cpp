#include "btinterface.h"

#include <QBluetoothSocket>
#include <QTimer>

CBTInterface::CBTInterface(QObject *parent) : QObject(parent)
{
    bluetoothSocket =
            new QtMobility::QBluetoothSocket(QtMobility::QBluetoothSocket::RfcommSocket,
                                             this);
    connect(bluetoothSocket, SIGNAL(connected()), SLOT(btConnected()));
    connect(bluetoothSocket, SIGNAL(disconnected()), SLOT(btDisconnected()));
    connect(bluetoothSocket, SIGNAL(readyRead()), SLOT(btHasData()));

    btSendTimer = new QTimer(this);
    btSendTimer->setInterval(BTQUEUE_DELAY);
    connect(btSendTimer, SIGNAL(timeout()), SLOT(btSendFromQueue()));
}

void CBTInterface::btConnected()
{
    qDebug() << "Got Bluetooth connection!";
    btSendTimer->start();
    emit connected();
}

void CBTInterface::btDisconnected()
{
    qDebug() << "Lost bt connection";
    btSendTimer->stop();
    btSendQueue.clear();
    emit disconnected();
}

void CBTInterface::btHasData()
{
    qDebug() << "Got some bt data:" << bluetoothSocket->bytesAvailable() << "bytes";
    bluetoothSocket->readAll();
    /*while (bluetoothSocket->canReadLine())
        qDebug() << bluetoothSocket->readLine();*/
//    qDebug() << "Data:" << bluetoothSocket->readAll();
}

void CBTInterface::btSendFromQueue()
{
    if (bluetoothSocket->state() != QtMobility::QBluetoothSocket::ConnectedState)
        btSendTimer->stop();
    else if (!btSendQueue.isEmpty())
        bluetoothSocket->write(btSendQueue.dequeue());
}

void CBTInterface::connectBT()
{
    bluetoothSocket->connectToService(QtMobility::QBluetoothAddress("00:11:67:AD:CD:BB"),
                                      QtMobility::QBluetoothUuid::Rfcomm);
}

void CBTInterface::disconnectBT()
{
    bluetoothSocket->disconnectFromService();
}

bool CBTInterface::isConnected() const
{
    return (bluetoothSocket->state() == QtMobility::QBluetoothSocket::ConnectedState);
}
