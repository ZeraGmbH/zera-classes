#ifndef IOTRANSFERDATAGROUP_H
#define IOTRANSFERDATAGROUP_H

#include "iotransferdatasingle.h"
#include "io-ids/ioidgenerator.h"

typedef QList<IoTransferDataSingle::Ptr> tIoTransferList;

class IoTransferDataGroup
{
public:
    enum SourceGroupTypes { // don't forget unittest on add
        GROUP_TYPE_UNDEFINED = 0,
        GROUP_TYPE_SWITCH_ON,
        GROUP_TYPE_SWITCH_OFF,
        GROUP_TYPE_STATE_POLL,
        GROUP_TYPE_UNDEF_BOTTOM
    };

    enum GroupErrorBehaviors {
        BEHAVE_UNDEFINED = 0,
        BEHAVE_STOP_ON_ERROR,
        BEHAVE_CONTINUE_ON_ERROR,
        BEHAVE_UNDEF_BOTTOM
    };
    IoTransferDataGroup();
    IoTransferDataGroup(SourceGroupTypes groupType, GroupErrorBehaviors errorBehavior);

    void appendTransferList(tIoTransferList transferList);

    bool passedAll() const;
    void evalAll();

    bool isSwitchGroup() const;
    bool isStateQueryGroup() const;
    SourceGroupTypes getGroupType() const;
    int getGroupId() const;
    GroupErrorBehaviors getErrorBehavior() const;

    int getTransferCount();
    IoTransferDataSingle::Ptr getTransfer(int idx);

    // Currently just for unittests
    bool operator == (const IoTransferDataGroup& other);
    bool operator != (const IoTransferDataGroup& other);

    void setDemoErrorOnTransfer(int idx);
private:
    tIoTransferList m_ioTransferList;
    SourceGroupTypes m_groupType = GROUP_TYPE_UNDEFINED;
    GroupErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    static IoIdGenerator m_idGenerator;
    int m_groupId = 0;
    bool m_bPassedAll = false;
};
Q_DECLARE_METATYPE(IoTransferDataGroup)


#endif // IOTRANSFERDATAGROUP_H
