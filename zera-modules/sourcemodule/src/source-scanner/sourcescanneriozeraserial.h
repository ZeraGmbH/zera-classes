#ifndef SOURCESCANNERACTIONZERASERIAL_H
#define SOURCESCANNERACTIONZERASERIAL_H

#include "sourcescannergenandhandleio.h"

class SourceScannerIoZeraSerial : public ISourceScannerGenAndHandleIo
{
public:
    SourceScannerIoZeraSerial();
    virtual QList<IoQueueEntry::Ptr> getScanIoGroups() override;
    virtual SourceProperties evalResponses(int ioGroupId) override;

private:
    IoQueueEntry::Ptr getCleanupUnfinishedGroup();
    IoQueueEntry::Ptr getDeviceScanGroup();
    int findGroupIdx(int ioGroupId);
    QByteArray extractVersionFromResponse(IoTransferDataSingle::Ptr ioData);
    QByteArray extractNameFromResponse(IoTransferDataSingle::Ptr ioData, QByteArray version, QByteArray cmdSend);
    SourceProperties extractProperties(IoTransferDataSingle::Ptr ioData, int idxInScanList);
    SourceProperties evalResponsesForTransactionGroup(IoQueueEntry::Ptr group);

    QList<IoQueueEntry::Ptr> m_scanIoGroupList;
};

#endif // SOURCESCANNERACTIONZERASERIAL_H
