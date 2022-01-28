#ifndef SOURCESCANNERACTIONDEMO_H
#define SOURCESCANNERACTIONDEMO_H

#include "sourcescannergenandhandleio.h"

class SourceScannerIoDemo : public ISourceScannerGenAndHandleIo
{
public:
    virtual QList<IoQueueEntry::Ptr> getIoQueueEntriesForScan() override;
    virtual SourceProperties evalResponses(int ioGroupId) override;

private:
    static SupportedSourceTypes getNextSourceType();
};

#endif // SOURCESCANNERACTIONDEMO_H
