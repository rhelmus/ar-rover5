#include "tcpclientinterface.h"

CTcpClientInterface::CTcpClientInterface(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readTcpData()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
}

void CTcpClientInterface::connectToServer(const QString &s)
{
    tcpSocket->abort(); // Always disconnect first
    tcpSocket->connectToHost(s, 40001);
}

void CTcpClientInterface::send(const QByteArray &data)
{
    if (!isConnected())
        return;

    tcpSocket->write(data);
}

void CTcpClientInterface::handleDisconnect()
{
    recBuffer.clear();
    emit disconnected();
}

void CTcpClientInterface::readTcpData()
{
    // message format:
    //  0: Start marker
    //  1: Msg size
    //  2: Msg type
    //  3 .. n-1: Data
    //  n: End marker

    recBuffer += tcpSocket->readAll();

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
}

void CTcpClientInterface::handleSocketError(QAbstractSocket::SocketError)
{
    recBuffer.clear();
    emit tcpError(tcpSocket->errorString());
}


CTcpClientMessage::CTcpClientMessage(EMessage m)
{
    data.push_back(static_cast<char>(MSG_STARTMARKER));
    data.push_back(static_cast<char>(0)); // Message size (to be filled in)
    data.push_back(static_cast<char>(m));
}

CTcpClientMessage::CTcpClientMessage()
{
    data.push_back(static_cast<char>(MSG_STARTMARKER));
    data.push_back(static_cast<char>(0)); // Message size (to be filled in)
    data.push_back(static_cast<char>(MSG_NONE)); // to be filled in
}

CTcpClientMessage &CTcpClientMessage::operator <<(uint16_t i)
{
    uint8_t buf[2];
    intToBytes(i, buf);
    data.push_back(buf[0]);
    data.push_back(buf[1]);
    return *this;
}

CTcpClientMessage::operator QByteArray()
{
    data[1] = data.size() - 2; // Minus size and start marker
    data.push_back(static_cast<char>(MSG_ENDMARKER));
    return data;
}
