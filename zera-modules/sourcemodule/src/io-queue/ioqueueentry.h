#ifndef IOQUEUEGROUP_H
#define IOQUEUEGROUP_H

#include "io-device/iotransferdatasingle.h"
#include "transaction-ids/idgenerator.h"
#include <QSharedPointer>

typedef QList<IoTransferDataSingle::Ptr> tIoTransferList;

class IoQueueEntry
{
public:
    enum GroupErrorBehaviors {
        BEHAVE_UNDEFINED = 0,
        BEHAVE_STOP_ON_ERROR,
        BEHAVE_CONTINUE_ON_ERROR,
        BEHAVE_STOP_ON_FIRST_OK,
        BEHAVE_UNDEF_BOTTOM
    };

    typedef QSharedPointer<IoQueueEntry> Ptr;

    IoQueueEntry(GroupErrorBehaviors errorBehavior);

    void appendTransferList(tIoTransferList transferList);

    bool passedAll() const;
    void evalAll();

    int getGroupId() const;
    GroupErrorBehaviors getErrorBehavior() const;

    int getTransferCount();
    IoTransferDataSingle::Ptr getTransfer(int idx);

    void setDemoErrorOnTransfer(int idx);
private:
    tIoTransferList m_ioTransferList;
    GroupErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    static IoIdGenerator m_idGenerator;
    int m_groupId = 0;
    bool m_bPassedAll = false;
};

Q_DECLARE_METATYPE(IoQueueEntry::Ptr)

#endif // IOQUEUEGROUP_H
