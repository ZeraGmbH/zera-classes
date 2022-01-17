#ifndef CSOURCEIOPACKETGENERATOR_H
#define CSOURCEIOPACKETGENERATOR_H

#include <QByteArray>
#include <QJsonObject>
#include "sourceactions.h"
#include "json/jsonstructapi.h"

enum SourceCommandTypes { // don't forget unittest on add
    COMMAND_UNDEFINED = 0,
    COMMAND_SWITCH_ON,
    COMMAND_SWITCH_OFF,
    COMMAND_STATE_POLL,
    COMMAND_UNDEF_BOTTOM
};

enum PacketErrorBehaviors {
    BEHAVE_UNDEFINED = 0,
    BEHAVE_STOP_ON_ERROR,
    BEHAVE_CONTINUE_ON_ERROR,
    BEHAVE_UNDEF_BOTTOM
};

class IoSingleOutIn
{
public:
    IoSingleOutIn() {}
    IoSingleOutIn(QByteArray bytesSend,
                  QByteArray bytesExpectedLead,
                  QByteArray bytesExpectedTrail = "\r",
                  int responseTimeoutMs = 0) :
        m_actionType(SourceActionTypes::ActionTypes(0)),
        m_responseTimeoutMs(responseTimeoutMs),
        m_bytesSend(bytesSend),
        m_bytesExpectedLead(bytesExpectedLead),
        m_bytesExpectedTrail(bytesExpectedTrail)
    {}
    void setActionType(SourceActionTypes::ActionTypes actionType);
    bool operator == (const IoSingleOutIn& other);

    SourceActionTypes::ActionTypes m_actionType = SourceActionTypes::SWITCH_UNDEF;
    int m_responseTimeoutMs = 0;
    QByteArray m_bytesSend;
    QByteArray m_bytesExpectedLead;
    QByteArray m_bytesExpectedTrail;
};

typedef QList<IoSingleOutIn> tIoOutInList;

class IoCommandPacket
{
public:
    SourceCommandTypes m_commandType = COMMAND_UNDEFINED;
    PacketErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    tIoOutInList m_outInList;
};

class IoPacketGenerator
{
public:
    IoPacketGenerator(JsonStructApi jsonStructApi);
    ~IoPacketGenerator();

    // single
    tIoOutInList generateListForAction(SourceActionTypes::ActionTypes actionType);
    // composed
    IoCommandPacket generateOnOffPacket(JsonParamApi requestedParams);
    IoCommandPacket generateStatusPollPacket();
private:
    tIoOutInList generateRMSAndAngleUList();
    tIoOutInList generateRMSAndAngleIList();
    tIoOutInList generateSwitchPhasesList();
    tIoOutInList generateFrequencyList();
    tIoOutInList generateRegulationList();

    tIoOutInList generateQueryStatusList();
    tIoOutInList generateQueryActualList();

    JsonStructApi m_jsonStructApi;
    JsonParamApi m_paramsRequested;

    QByteArray m_ioPrefix;
};


class IoCmdFormatHelper
{
public:
    static QByteArray formatDouble(double val, int preDigits, char digit, int postDigits);
};

#endif // CSOURCEIOPACKETGENERATOR_H
