#include "ioqueueentrylist.h"

int IoQueueEntryListFind::findGroupIdx(const IoQueueEntryList &list, int ioGroupId)
{
    int idxFound = -1;
    for(int idx=0; idx<list.count(); ++idx) {
        if(list[idx]->getGroupId() == ioGroupId) {
            idxFound = idx;
            break;
        }
    }
    return idxFound;
}

IoQueueEntry::Ptr IoQueueEntryListFind::findGroup(const IoQueueEntryList &list, int ioGroupId)
{
    IoQueueEntry::Ptr groupFond;
    int idxFound = findGroupIdx(list, ioGroupId);
    if(idxFound >= 0) {
        groupFond = list[idxFound];
    }
    return groupFond;
}
