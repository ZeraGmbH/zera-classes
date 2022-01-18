#ifndef IOMULTIPLETRANSFERGROUP_H
#define IOMULTIPLETRANSFERGROUP_H

#include "iosingletransferdata.h"

enum SourceGroupTypes { // don't forget unittest on add
    COMMAND_UNDEFINED = 0,
    COMMAND_SWITCH_ON,
    COMMAND_SWITCH_OFF,
    COMMAND_STATE_POLL,
    COMMAND_UNDEF_BOTTOM
};

enum GroupErrorBehaviors {
    BEHAVE_UNDEFINED = 0,
    BEHAVE_STOP_ON_ERROR,
    BEHAVE_CONTINUE_ON_ERROR,
    BEHAVE_UNDEF_BOTTOM
};

class IoMultipleTransferGroup
{
public:
    bool passedAll() const;
    void evalAll();

    bool isSwitchGroup() const;
    bool isStateQueryGroup() const;

    bool operator == (const IoMultipleTransferGroup& other);

    int m_groupId = 0;
    SourceGroupTypes m_commandType = COMMAND_UNDEFINED;
    GroupErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    tIoTransferList m_ioTransferList;
private:
    bool m_bPassedAll = false;
};
Q_DECLARE_METATYPE(IoMultipleTransferGroup)


#endif // IOMULTIPLETRANSFERGROUP_H
