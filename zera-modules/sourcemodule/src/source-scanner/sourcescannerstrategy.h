#ifndef SOURCESCANNERSTRATEGY_H
#define SOURCESCANNERSTRATEGY_H

#include "io-queue/ioqueueentry.h"
#include "source-device/sourceproperties.h"
#include <QList>

class ISourceScannerStrategy
{
public:
    typedef QSharedPointer<ISourceScannerStrategy> Ptr;

    virtual QList<IoQueueEntry::Ptr> getIoQueueEntriesForScan() = 0;
    virtual SourceProperties evalResponses(int ioGroupId) = 0;
};

#endif // SOURCESCANNERSTRATEGY_H
