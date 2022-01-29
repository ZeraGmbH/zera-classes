#ifndef SOURCESCANNERACTIONDEMO_H
#define SOURCESCANNERACTIONDEMO_H

#include "sourcescannerstrategy.h"

class SourceScannerIoDemo : public ISourceScannerStrategy
{
public:
    SourceScannerIoDemo();
    virtual IoQueueEntryList getIoQueueEntriesForScan() override;
    virtual SourceProperties evalResponses(int ioGroupId) override;

private:
    static SupportedSourceTypes getNextSourceType();

    IoQueueEntryList m_scanIoGroupList;
};

#endif // SOURCESCANNERACTIONDEMO_H
