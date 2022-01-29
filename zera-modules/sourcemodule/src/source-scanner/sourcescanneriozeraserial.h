#ifndef SOURCESCANNERACTIONZERASERIAL_H
#define SOURCESCANNERACTIONZERASERIAL_H

#include "sourcescannerstrategy.h"

class SourceScannerIoZeraSerial : public ISourceScannerStrategy
{
public:
    SourceScannerIoZeraSerial();
    virtual IoQueueEntryList getIoQueueEntriesForScan() override;
    virtual SourceProperties evalResponses(int ioGroupId) override;

private:
    IoQueueEntry::Ptr getCleanupUnfinishedGroup();
    IoQueueEntry::Ptr getDeviceScanGroup();
    QByteArray extractVersionFromResponse(IoTransferDataSingle::Ptr ioData);
    QByteArray extractNameFromResponse(IoTransferDataSingle::Ptr ioData, QByteArray version, QByteArray cmdSend);
    SourceProperties extractProperties(IoTransferDataSingle::Ptr ioData, int idxInScanList);
    SourceProperties evalResponsesForTransactionGroup(IoQueueEntry::Ptr group);

    IoQueueEntryList m_scanIoGroupList;
};

#endif // SOURCESCANNERACTIONZERASERIAL_H
