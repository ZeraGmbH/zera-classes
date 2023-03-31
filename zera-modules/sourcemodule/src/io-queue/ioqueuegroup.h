#ifndef IOQUEUEGROUP_H
#define IOQUEUEGROUP_H

#include "ioqueuebehaviors.h"
#include "iotransferdatasingle.h"
#include "idgenerator.h"
#include <QSharedPointer>

typedef QList<IoTransferDataSingle::Ptr> tIoTransferList;

class IoQueueGroup
{
public:
    typedef QSharedPointer<IoQueueGroup> Ptr;
    IoQueueGroup(IoQueueErrorBehaviors errorBehavior);
    void appendTransferList(tIoTransferList transferList);
    bool passedAll() const;
    int getGroupId() const;
    IoQueueErrorBehaviors getErrorBehavior() const;
    int getTransferCount();
    IoTransferDataSingle::Ptr getTransfer(int idx);

private:
    tIoTransferList m_ioTransferList;
    IoQueueErrorBehaviors m_errorBehavior;
    static IoIdGenerator m_idGenerator;
    int m_groupId = 0;
};

Q_DECLARE_METATYPE(IoQueueGroup::Ptr)

#endif // IOQUEUEGROUP_H
