#ifndef SOURCESCANNERSTRATEGY_H
#define SOURCESCANNERSTRATEGY_H

#include "ioqueuegrouplistfind.h"
#include "ioqueuegroup.h"
#include "sourceproperties.h"
#include <QList>

class ISourceScannerStrategy
{
public:
    typedef QSharedPointer<ISourceScannerStrategy> Ptr;

    virtual IoQueueGroupListPtr getIoQueueGroupsForScan() = 0;
    virtual SourceProperties evalResponses(int ioGroupId) = 0;
};

#endif // SOURCESCANNERSTRATEGY_H
