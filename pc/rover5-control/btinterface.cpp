#include "btinterface.h"

#include <QApplication>
#include <QBluetoothSocket>
#include <QDebug>
#include <QTime>
#include <QTimer>

CBTInterface::CBTInterface(QObject *parent) : QObject(parent), bluetoothSocket(0)
{
    /*bluetoothSocket =
            new QtMobility::QBluetoothSocket(QtMobility::QBluetoothSocket::RfcommSocket,
                                             this);
    connect(bluetoothSocket, SIGNAL(connected()), SLOT(btConnected()));
    connect(bluetoothSocket, SIGNAL(disconnected()), SLOT(btDisconnected()));
    connect(bluetoothSocket, SIGNAL(readyRead()), SLOT(btHasData()));*/

    btSendTimer = new QTimer(this);
    btSendTimer->setInterval(BTQUEUE_DELAY);
    connect(btSendTimer, SIGNAL(timeout()), SLOT(btSendFromQueue()));
}

void CBTInterface::doBtDisconnect()
{
    if (bluetoothSocket)
    {
        bluetoothSocket->disconnectFromService();
        bluetoothSocket->deleteLater();
        bluetoothSocket = 0;
    }
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

//    qDebug() << "Got some bt data:" << bluetoothSocket->bytesAvailable() << "bytes";

    recBuffer += bluetoothSocket->readAll();

    int msgstartind = recBuffer.indexOf(MSG_STARTMARKER);
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
        if (recBuffer.at(msgsize + extramsgsize - 1) == MSG_ENDMARKER)
        {
            emit msgReceived(static_cast<EMessage>((int)recBuffer[2]),
                    recBuffer.mid(3, msgsize-1));
        }

        recBuffer.remove(0, msgsize + extramsgsize);
        msgstartind = recBuffer.indexOf(MSG_STARTMARKER);
    }

    /*while (bluetoothSocket->canReadLine())
        qDebug() << bluetoothSocket->readLine();*/
//    qDebug() << "Data:" << bluetoothSocket->readAll();
}

void CBTInterface::btSendFromQueue()
{
    if (!isConnected())
        btSendTimer->stop();
    else if (!btSendQueue.isEmpty())
        bluetoothSocket->write(btSendQueue.dequeue());
}

void CBTInterface::connectBT()
{
    doBtDisconnect();

    // There seems to be a bug in QtMobility causing it to crash on reconnect,
    // deleting & constructing new sockets everytime seems to be a work around
    if (!bluetoothSocket)
    {
        bluetoothSocket =
                new QtMobility::QBluetoothSocket(QtMobility::QBluetoothSocket::RfcommSocket,
                                                 this);
        connect(bluetoothSocket, SIGNAL(connected()), SLOT(btConnected()));
        connect(bluetoothSocket, SIGNAL(disconnected()), SLOT(btDisconnected()));
        connect(bluetoothSocket, SIGNAL(readyRead()), SLOT(btHasData()));
    }

    bluetoothSocket->connectToService(QtMobility::QBluetoothAddress("00:11:67:AD:CD:BB"),
                                      QtMobility::QBluetoothUuid::Rfcomm);
}

void CBTInterface::disconnectBT()
{
    if (bluetoothSocket)
    {
        qDebug() << "Sending disconnect";
        send(CBTMessage(MSG_CNTRL_DISCONNECT));

        // Flush
        while (!btSendQueue.isEmpty())
            bluetoothSocket->write(btSendQueue.dequeue());

        QTimer::singleShot(2000, this, SLOT(doBtDisconnect()));
        /*QTime waittime;
        waittime.start();
        while (waittime.elapsed() < 5000)
            qApp->processEvents();

        bluetoothSocket->disconnectFromService();

        bluetoothSocket->deleteLater();
        bluetoothSocket = 0;*/
    }
}

bool CBTInterface::isConnected() const
{
    return (bluetoothSocket && (bluetoothSocket->state() == QtMobility::QBluetoothSocket::ConnectedState));
}

void CBTInterface::send(const QByteArray &data)
{
    if (!isConnected())
        return;

    btSendQueue.enqueue(data);
}


CBTMessage::CBTMessage(EMessage m)
{
    data.push_back(static_cast<char>(MSG_STARTMARKER));
    data.push_back(static_cast<char>(0)); // Message size (to be filled in)
    data.push_back(static_cast<char>(m));
}

CBTMessage::CBTMessage()
{
    data.push_back(static_cast<char>(MSG_STARTMARKER));
    data.push_back(static_cast<char>(0)); // Message size (to be filled in)
    data.push_back(static_cast<char>(MSG_NONE)); // to be filled in
}

CBTMessage &CBTMessage::operator <<(uint16_t i)
{
    uint8_t buf[2];
    intToBytes(i, buf);
    data.push_back(buf[0]);
    data.push_back(buf[1]);
    return *this;
}

CBTMessage::operator QByteArray()
{
    data[1] = data.size() - 2; // Minus size and start marker
    data.push_back(static_cast<char>(MSG_ENDMARKER));
    return data;
}
