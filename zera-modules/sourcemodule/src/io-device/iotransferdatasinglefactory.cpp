#include "iotransferdatasinglefactory.h"

tIoTransferDataSingleShPtr IoTransferDataSingleFactory::createIoData()
{
    return tIoTransferDataSingleShPtr(new IoTransferDataSingle());
}

tIoTransferDataSingleShPtr IoTransferDataSingleFactory::createIoData(QByteArray bytesSend,
                                                                     QByteArray bytesExpectedLead,
                                                                     QByteArray bytesExpectedTrail,
                                                                     int responseTimeoutMs,
                                                                     bool demoErrorResponse)
{
    return tIoTransferDataSingleShPtr(new IoTransferDataSingle(bytesSend,
                                                               bytesExpectedLead,
                                                               bytesExpectedTrail,
                                                               responseTimeoutMs,
                                                               demoErrorResponse));
}
