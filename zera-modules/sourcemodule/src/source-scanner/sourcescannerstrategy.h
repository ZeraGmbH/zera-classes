#ifndef SOURCESCANNERSTRATEGY_H
#define SOURCESCANNERSTRATEGY_H

#include "io-queue/ioqueueentry.h"
#include "io-queue/ioqueueentrylist.h"
#include "source-device/sourceproperties.h"
#include <QList>

class ISourceScannerStrategy
{
public:
    typedef QSharedPointer<ISourceScannerStrategy> Ptr;

    virtual IoQueueEntryList getIoQueueEntriesForScan() = 0;
    virtual SourceProperties evalResponses(int ioGroupId) = 0;
};

#endif // SOURCESCANNERSTRATEGY_H
