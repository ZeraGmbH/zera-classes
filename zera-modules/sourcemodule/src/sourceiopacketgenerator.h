#ifndef CSOURCEIOPACKETGENERATOR_H
#define CSOURCEIOPACKETGENERATOR_H

#include <QByteArray>
#include <QJsonObject>
#include "sourceactions.h"
#include "json/jsonstructapi.h"

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

class SourceSingleOutIn
{
public:
    SourceSingleOutIn() {}
    SourceSingleOutIn(SourceIoResponseTypes ioResponseType,
                       QByteArray bytesSend,
                       QByteArray bytesExpected,
                       int responseTimeoutMs = 0) :
        m_actionType(SourceActionTypes::ActionTypes(0)),
        m_responseType(ioResponseType),
        m_responseTimeoutMs(responseTimeoutMs),
        m_bytesSend(bytesSend),
        m_bytesExpected(bytesExpected)
    {}
    void setActionType(SourceActionTypes::ActionTypes actionType);
    bool operator == (const SourceSingleOutIn& other);

    SourceActionTypes::ActionTypes m_actionType = SourceActionTypes::SWITCH_UNDEF;
    SourceIoResponseTypes m_responseType = RESP_UNDEFINED;
    int m_responseTimeoutMs = 0;
    QByteArray m_bytesSend;
    QByteArray m_bytesExpected;
};

typedef QList<SourceSingleOutIn> tSourceOutInList;

class SourceCommandPacket
{
public:
    SourceCommandTypes m_commandType = COMMAND_UNDEFINED;
    SourcePacketErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    tSourceOutInList m_outInList;
};

class SourceIoPacketGenerator
{
public:
    SourceIoPacketGenerator(QJsonObject jsonParamsStructure);
    ~SourceIoPacketGenerator();

    // single
    tSourceOutInList generateListForAction(SourceActionTypes::ActionTypes actionType);
    // composed
    SourceCommandPacket generateOnOffPacket(JsonParamApi requestedParams);
    SourceCommandPacket generateStatusPollPacket();
private:
    tSourceOutInList generateRMSAndAngleUList();
    tSourceOutInList generateRMSAndAngleIList();
    tSourceOutInList generateSwitchPhasesList();
    tSourceOutInList generateFrequencyList();
    tSourceOutInList generateRegulationList();

    tSourceOutInList generateQueryStatusList();
    tSourceOutInList generateQueryActualList();

    JsonStructApi* m_jsonStructApi = nullptr;
    JsonParamApi m_paramsRequested;

    QByteArray m_ioPrefix;
};


class SourceIoCmdFormatHelper
{
public:
    static QByteArray formatDouble(double val, int preDigits, char digit, int postDigits);
};

#endif // CSOURCEIOPACKETGENERATOR_H
