#ifndef IOQUEUEGROUP_H
#define IOQUEUEGROUP_H

#include "ioqueuebehaviors.h"
#include "io-device/iotransferdatasingle.h"
#include "transaction-ids/idgenerator.h"
#include <QSharedPointer>

typedef QList<IoTransferDataSingle::Ptr> tIoTransferList;

class IoQueueEntry
{
public:
    typedef QSharedPointer<IoQueueEntry> Ptr;
    IoQueueEntry(IoQueueErrorBehaviors errorBehavior);
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

Q_DECLARE_METATYPE(IoQueueEntry::Ptr)

#endif // IOQUEUEGROUP_H
