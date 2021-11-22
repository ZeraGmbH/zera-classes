#ifndef CSOURCEIOPACKETGENERATOR_H
#define CSOURCEIOPACKETGENERATOR_H

#include <QByteArray>
#include <QJsonObject>
#include "sourceactions.h"
#include "sourcejsonapi.h"

enum SourceCommandTypes { // don't forget unittest on add
    COMMAND_UNDEFINED = 0,
    COMMAND_SWITCH,
    COMMAND_STATE_POLL,
    COMMAND_UNDEF_BOTTOM
};

enum SourcePacketErrorBehaviors {
    BEHAVE_UNDEFINED = 0,
    BEHAVE_STOP_ON_ERROR,
    BEHAVE_CONTINUE_ON_ERROR,
    BEHAVE_UNDEF_BOTTOM
};

enum SourceIoResponseTypes {
    RESP_UNDEFINED = 0,
    RESP_FULL_DATA_SEQUENCE,
    RESP_PART_DATA_SEQUENCE,
    RESP_UNDEF_BOTTOM
};

class cSourceSingleOutIn
{
public:
    cSourceSingleOutIn() {}
    cSourceSingleOutIn(SourceIoResponseTypes ioResponseType,
                         QByteArray bytesSend,
                         QByteArray bytesExpected) :
        m_actionType(cSourceActionTypes::ActionTypes(0)),
        m_responseType(ioResponseType),
        m_bytesSend(bytesSend),
        m_bytesExpected(bytesExpected)
    {}
    void setActionType(cSourceActionTypes::ActionTypes actionType);
    bool operator == (const cSourceSingleOutIn& other);

    cSourceActionTypes::ActionTypes m_actionType = cSourceActionTypes::SWITCH_UNDEF;
    SourceIoResponseTypes m_responseType = RESP_UNDEFINED;
    QByteArray m_bytesSend;
    QByteArray m_bytesExpected;
};

typedef QList<cSourceSingleOutIn> tSourceOutInList;

class cSourceCommandPacket
{
public:
    SourceCommandTypes m_commandType = COMMAND_UNDEFINED;
    SourcePacketErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    tSourceOutInList m_outInList;
};

class cSourceIoPacketGenerator
{
public:
    cSourceIoPacketGenerator(QJsonObject jsonParamsStructure);
    ~cSourceIoPacketGenerator();

    // single
    tSourceOutInList generateListForAction(cSourceActionTypes::ActionTypes actionType);
    // composed
    cSourceCommandPacket generateOnOffPacket(cSourceJsonParamApi requestedParams);
    cSourceCommandPacket generateStatusPollPacket();
private:
    tSourceOutInList generateRMSAndAngleUList();
    tSourceOutInList generateRMSAndAngleIList();
    tSourceOutInList generateSwitchPhasesList();
    tSourceOutInList generateFrequencyList();
    tSourceOutInList generateRegulationList();

    tSourceOutInList generateQueryStatusList();
    tSourceOutInList generateQueryActualList();

    cSourceJsonStructApi* m_jsonStructApi = nullptr;
    cSourceJsonParamApi m_paramsRequested;

    QByteArray m_ioPrefix;
};


class cSourceIoCmdHelper
{
public:
    static QByteArray formatDouble(double val, int preDigits, char digit, int postDigits);
};

#endif // CSOURCEIOPACKETGENERATOR_H
