#ifndef SOURCESCANNERACTIONDECORATOR_H
#define SOURCESCANNERACTIONDECORATOR_H

#include "io-queue/ioqueueentry.h"
#include "source-device/sourceproperties.h"
#include <QList>

class ISourceScannerGenAndHandleIo
{
public:
    typedef QSharedPointer<ISourceScannerGenAndHandleIo> Ptr;

    virtual QList<IoQueueEntry::Ptr> getScanIoGroups() = 0;
    virtual SourceProperties evalResponses(int ioGroupId) = 0;
};

#endif // SOURCESCANNERACTIONDECORATOR_H
