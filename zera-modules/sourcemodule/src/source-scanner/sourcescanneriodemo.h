#ifndef SOURCESCANNERACTIONDEMO_H
#define SOURCESCANNERACTIONDEMO_H

#include "sourcescannerstrategy.h"

class SourceScannerIoDemo : public ISourceScannerStrategy
{
public:
    virtual QList<IoQueueEntry::Ptr> getIoQueueEntriesForScan() override;
    virtual SourceProperties evalResponses(int ioGroupId) override;

private:
    static SupportedSourceTypes getNextSourceType();
};

#endif // SOURCESCANNERACTIONDEMO_H
