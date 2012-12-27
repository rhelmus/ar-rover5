#ifndef TCPUTILS_H
#define TCPUTILS_H

#include "shared.h"

#include <QByteArray>
#include <QDataStream>

class CTcpMsgComposer
{
    QByteArray block;
    QDataStream *dataStream;

public:
    CTcpMsgComposer(EMessage msg);
    ~CTcpMsgComposer(void);

    template <typename C> CTcpMsgComposer &operator <<(const C &data)
    {
        *dataStream << data;
        return *this;
    }

    operator QByteArray(void);
};

#endif // TCPUTILS_H
