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
    recBuffer.clear();
    emit disconnected();
}

void CBTInterface::btHasData()
{
    // BT message format:
    //  0: Start marker
    //  1: Msg size
    //  2: Msg type
    //  3 .. n-1: Data
    //  n: End marker

    qDebug() << "Got some bt data:" << bluetoothSocket->bytesAvailable() << "bytes";

    recBuffer += bluetoothSocket->readAll();

    int msgstartind = recBuffer.indexOf(MSG_BT_STARTMARKER);
    if (msgstartind == -1)
    {
        recBuffer.clear(); // No message start detected, remove dirt
        return; // wait untill we got something useful
    }

    const int extramsgsize = 3; // start- and end marker and msg size
    while (msgstartind != -1)
    {
        if (msgstartind > 0) // dirt in between?
        {
            recBuffer.remove(0, msgstartind);
            msgstartind = 0;
        }

        if (recBuffer.size() < extramsgsize)
            return; // wait for more

        const uint16_t msgsize = recBuffer[1];
        if (recBuffer.size() < (msgsize + extramsgsize))
            return; // wait for more

        // End marker present?
        if (recBuffer.at(msgsize + extramsgsize - 1) == MSG_BT_ENDMARKER)
        {
            emit msgReceived(static_cast<EMessage>((int)recBuffer[2]),
                    recBuffer.mid(3, msgsize-1));
        }

        recBuffer.remove(0, msgsize + extramsgsize);
        msgstartind = recBuffer.indexOf(MSG_BT_STARTMARKER);
    }

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
