#ifndef SOURCESCANNERACTIONZERASERIAL_H
#define SOURCESCANNERACTIONZERASERIAL_H

#include "sourcescannerstrategy.h"

class SourceScannerIoZeraSerial : public ISourceScannerStrategy
{
public:
    SourceScannerIoZeraSerial();
    virtual IoQueueGroupListPtr getIoQueueGroupsForScan() override;
    virtual SourceProperties evalResponses(IoQueueGroup::Ptr transferGroup) override;

private:
    IoQueueGroup::Ptr getCleanupUnfinishedIoCmdGroup();
    IoQueueGroup::Ptr getDeviceScanGroup();
    QByteArray extractVersionFromResponse(IoTransferDataSingle::Ptr ioData);
    QByteArray extractNameFromResponse(IoTransferDataSingle::Ptr ioData, QByteArray version, QByteArray cmdSend);
    SourceProperties extractProperties(IoTransferDataSingle::Ptr ioData, int idxInScanList);
    SourceProperties evalResponsesForTransactionGroup(IoQueueGroup::Ptr group);

    IoQueueGroupListPtr m_scanIoGroupList;
};

#endif // SOURCESCANNERACTIONZERASERIAL_H
