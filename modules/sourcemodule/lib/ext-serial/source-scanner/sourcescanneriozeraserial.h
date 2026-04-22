#ifndef SOURCESCANNERACTIONZERASERIAL_H
#define SOURCESCANNERACTIONZERASERIAL_H

#include "sourcescannertemplate.h"

class SourceScannerIoZeraSerial : public SourceScannerTemplate
{
public:
    SourceScannerIoZeraSerial();
private:
    virtual SourceProperties evalResponses(IoQueueGroup::Ptr ioGroup) override;
    IoQueueGroup::Ptr getCleanupUnfinishedIoCmdGroup();
    IoQueueGroup::Ptr getDeviceScanGroup();
    QByteArray extractVersionFromResponse(IoTransferDataSingle::Ptr ioData);
    QByteArray extractNameFromResponse(IoTransferDataSingle::Ptr ioData, QByteArray version, QByteArray cmdSend);
    SourceProperties extractProperties(IoTransferDataSingle::Ptr ioData, int idxInScanList);
    SourceProperties evalResponsesForTransactionGroup(IoQueueGroup::Ptr group);
    bool m_doNotEvaluateCleanupIO;
};

#endif // SOURCESCANNERACTIONZERASERIAL_H
