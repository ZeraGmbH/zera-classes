#ifndef SOURCESCANNERACTIONDEMO_H
#define SOURCESCANNERACTIONDEMO_H

#include "sourcescannerstrategy.h"

class SourceScannerIoDemo : public ISourceScannerStrategy
{
public:
    SourceScannerIoDemo();
    virtual IoQueueGroupListPtr getIoQueueGroupsForScan() override;
    virtual SourceProperties evalResponses(IoQueueGroup::Ptr transferGroup) override;

private:
    static SupportedSourceTypes getNextSourceType();

    IoQueueGroupListPtr m_scanIoGroupList;
};

#endif // SOURCESCANNERACTIONDEMO_H
