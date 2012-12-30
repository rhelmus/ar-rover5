#ifndef BTINTERFACE_H
#define BTINTERFACE_H

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

signals:
    void connected(void);
    void disconnected(void);
};

#endif // BTINTERFACE_H
