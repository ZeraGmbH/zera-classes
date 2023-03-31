#include "ioqueuegrouplistfind.h"

int IoQueueGroupListFind::findGroupIdx(const IoQueueGroupListPtr &list, int ioGroupId)
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

IoQueueGroup::Ptr IoQueueGroupListFind::findGroup(const IoQueueGroupListPtr &list, int ioGroupId)
{
    IoQueueGroup::Ptr groupFond;
    int idxFound = findGroupIdx(list, ioGroupId);
    if(idxFound >= 0) {
        groupFond = list[idxFound];
    }
    return groupFond;
}
