#ifndef BTINTERFACE_H
#define BTINTERFACE_H

#include "../../shared/shared.h"

#include <QByteArray>
#include <QObject>
#include <QQueue>

namespace QtMobility {
class QBluetoothSocket;
}

class QTimer;

class CBTInterface : public QObject
{
    Q_OBJECT

    enum { BTQUEUE_DELAY = 100 };

    QtMobility::QBluetoothSocket *bluetoothSocket;
    QQueue<QByteArray> btSendQueue;
    QTimer *btSendTimer;
    QByteArray recBuffer;

private slots:
    void btConnected(void);
    void btDisconnected(void);
    void btHasData(void);
    void btSendFromQueue(void);

public:
    explicit CBTInterface(QObject *parent = 0);

    void connectBT(void);
    void disconnectBT(void);
    bool isConnected(void) const;
    void send(const QByteArray &data);

signals:
    void connected(void);
    void disconnected(void);
    void msgReceived(EMessage, QByteArray);
};

class CBTMessage
{
    QByteArray data;

public:
    CBTMessage(EMessage m);
    CBTMessage(void);

    void setMessage(EMessage m) { data[2] = static_cast<char>(m); }
    CBTMessage &operator <<(uint8_t b) { data.push_back(b); return *this; }
    CBTMessage &operator <<(uint16_t i);
    operator QByteArray(void);
};

#endif // BTINTERFACE_H
