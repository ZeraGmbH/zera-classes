#ifndef IOQUEUEENTRYLIST_H
#define IOQUEUEENTRYLIST_H

#include "ioqueuegroup.h"

typedef QList<IoQueueGroup::Ptr> IoQueueGroupListPtr;

class IoQueueGroupListFind
{
public:
    static int findGroupIdx(const IoQueueGroupListPtr& list, int ioGroupId);
    static IoQueueGroup::Ptr findGroup(const IoQueueGroupListPtr& list, int ioGroupId);
};

#endif // IOQUEUEENTRYLIST_H
