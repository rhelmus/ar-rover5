#include "tcputils.h"

CTcpMsgComposer::CTcpMsgComposer(EMessage msg)
{
    dataStream = new QDataStream(&block, QIODevice::WriteOnly);
    dataStream->setVersion(QDataStream::Qt_4_7);
    *dataStream << (uint32_t)0; // Reserve place for size
    *dataStream << (uint8_t)msg;
}

CTcpMsgComposer::~CTcpMsgComposer()
{
    delete dataStream;
}

CTcpMsgComposer::operator QByteArray()
{
    // Put in the size
    const uint64_t pos = dataStream->device()->pos();
    dataStream->device()->seek(0);
    *dataStream << (quint32)(block.size() - sizeof(uint32_t));
    dataStream->device()->seek(pos); // Restore old position
    return block;
}
