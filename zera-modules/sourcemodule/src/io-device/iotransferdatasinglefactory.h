#ifndef IOTRANSFERDATASINGLEFACTORY_H
#define IOTRANSFERDATASINGLEFACTORY_H

#include "iotransferdatasingle.h"

class IoTransferDataSingleFactory
{
public:
    static IoTransferDataSingle::Ptr createIoData();
    static IoTransferDataSingle::Ptr createIoData(
            QByteArray bytesSend,
            QByteArray bytesExpectedLead,
            QByteArray bytesExpectedTrail = "\r",
            int responseTimeoutMs = 0,
            bool demoErrorResponse = false);
};

#endif // IOTRANSFERDATASINGLEFACTORY_H
