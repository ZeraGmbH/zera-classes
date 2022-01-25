#ifndef IOTRANSFERDATAGROUP_H
#define IOTRANSFERDATAGROUP_H

#include "iotransferdatasingle.h"
#include "transaction-ids/idgenerator.h"
#include <QSharedPointer>

typedef QList<IoTransferDataSingle::Ptr> tIoTransferList;

class IoTransferDataGroup
{
public:
    enum GroupErrorBehaviors {
        BEHAVE_UNDEFINED = 0,
        BEHAVE_STOP_ON_ERROR,
        BEHAVE_CONTINUE_ON_ERROR,
        BEHAVE_UNDEF_BOTTOM
    };

    typedef QSharedPointer<IoTransferDataGroup> Ptr;

    IoTransferDataGroup(GroupErrorBehaviors errorBehavior);
    ~IoTransferDataGroup();

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

Q_DECLARE_METATYPE(IoTransferDataGroup::Ptr)

#endif // IOTRANSFERDATAGROUP_H
