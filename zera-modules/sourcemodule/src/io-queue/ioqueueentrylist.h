#ifndef IOQUEUEENTRYLIST_H
#define IOQUEUEENTRYLIST_H

#include "io-queue/ioqueueentry.h"

typedef QList<IoQueueEntry::Ptr> IoQueueEntryList;

class IoQueueEntryListFind
{
public:
    static int findGroupIdx(const IoQueueEntryList& list, int ioGroupId);
    static IoQueueEntry::Ptr findGroup(const IoQueueEntryList& list, int ioGroupId);
};

#endif // IOQUEUEENTRYLIST_H
