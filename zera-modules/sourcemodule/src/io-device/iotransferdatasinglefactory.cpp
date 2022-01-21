#include "iotransferdatasinglefactory.h"

IoTransferDataSingle::Ptr IoTransferDataSingleFactory::createIoData()
{
    return IoTransferDataSingle::Ptr(new IoTransferDataSingle());
}

IoTransferDataSingle::Ptr IoTransferDataSingleFactory::createIoData(QByteArray bytesSend,
                                                                     QByteArray bytesExpectedLead,
                                                                     QByteArray bytesExpectedTrail,
                                                                     int responseTimeoutMs,
                                                                     bool demoErrorResponse)
{
    return IoTransferDataSingle::Ptr(new IoTransferDataSingle(bytesSend,
                                                               bytesExpectedLead,
                                                               bytesExpectedTrail,
                                                               responseTimeoutMs,
                                                               demoErrorResponse));
}
