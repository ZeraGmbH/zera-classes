#ifndef CSOURCEIOTRANSACTIONGENERATOR_H
#define CSOURCEIOTRANSACTIONGENERATOR_H

#include <QByteArray>
#include <QJsonObject>
#include "sourceactions.h"
#include "sourcejsonapi.h"

enum SourceCommandTypes { // don't forget unittest on add
    COMMAND_UNDEFINED = 0,
    COMMAND_SWITCH,
    COMMAND_STATE_POLL
};

enum SourceResponseTypes {
    RESP_UNDEFINED = 0,
    RESP_WAIT_FOR_TIMEOUT,
    RESP_FULL_DATA_SEQUENCE,
    RESP_PART_DATA_SEQUENCE,

    RESP_LIMIT
};

class cSourceSingleOutIn
{
public:
    cSourceSingleOutIn() {}
    cSourceSingleOutIn(SourceResponseTypes ioResponseType,
                         QByteArray bytesSend,
                         QByteArray bytesExpected) :
        m_actionType(cSourceActionTypes::ActionTypes(0)),
        m_responseType(ioResponseType),
        m_bytesSend(bytesSend),
        m_bytesExpected(bytesExpected)
    {}
    void setActionType(cSourceActionTypes::ActionTypes actionType);

    cSourceActionTypes::ActionTypes m_actionType = cSourceActionTypes::SWITCH_UNDEF;
    SourceResponseTypes m_responseType = RESP_UNDEFINED;
    QByteArray m_bytesSend;
    QByteArray m_bytesExpected;
};

typedef QList<cSourceSingleOutIn> tSourceOutInList;

class cSourceCommandPacket
{
public:
    SourceCommandTypes m_commandType = COMMAND_UNDEFINED;
    tSourceOutInList m_singleOutInList;
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

    cSourceJsonStructApi* m_jsonStructApi = nullptr;
    cSourceJsonParamApi m_paramsRequested;

    QByteArray m_ioPrefix;
};


class cSourceIoCmdHelper
{
public:
    static QByteArray formatDouble(double val, int preDigits, char digit, int postDigits);
};

#endif // CSOURCEIOTRANSACTIONGENERATOR_H
