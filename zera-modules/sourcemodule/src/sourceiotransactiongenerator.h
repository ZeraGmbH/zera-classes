#ifndef CSOURCEIOTRANSACTIONGENERATOR_H
#define CSOURCEIOTRANSACTIONGENERATOR_H

#include <QByteArray>
#include <QJsonObject>
#include "sourceactions.h"
#include "sourcejsonparamapi.h"

enum SourceIoTransactionTypes {
    TRANSACTION_UNDEFINED = 0,
    TRANSACTION_SWITCH,
    TRANSACTION_STATE_POLL
};

enum SourceIoTransactionResponseTypes {
    RESP_UNDEFINED = 0,
    RESP_WAIT_FOR_TIMEOUT,
    RESP_FULL_DATA_SEQUENCE,
    RESP_PART_DATA_SEQUENCE,

    RESP_LIMIT
};

class cSourceIoTransaction
{
public:
    cSourceIoTransaction();
    cSourceIoTransaction(SourceIoTransactionResponseTypes ioResponseType,
                         QByteArray dataSend,
                         QByteArray dataExpectedResponse) :
        m_actionType(cSourceActionTypes::ActionTypes(0)),
        m_ioResponseType(ioResponseType),
        m_dataSend(dataSend),
        m_dataExpectedResponse(dataExpectedResponse)
    {}
    void setActionType(cSourceActionTypes::ActionTypes actionType);

    cSourceActionTypes::ActionTypes m_actionType;
    SourceIoTransactionResponseTypes m_ioResponseType;
    QByteArray m_dataSend;
    QByteArray m_dataExpectedResponse;
};

typedef QList<cSourceIoTransaction> tSourceIoTransactionList;

class cSourceIOTransactionPacket
{
public:
    SourceIoTransactionTypes m_transactionType;
    tSourceIoTransactionList m_transactionList;
};

class cSourceSwitchIoTransactionGenerator
{
public:
    cSourceSwitchIoTransactionGenerator(QJsonObject jsonParamsStructure);
    ~cSourceSwitchIoTransactionGenerator();

    // single
    tSourceIoTransactionList generateListForAction(cSourceActionTypes::ActionTypes actionType);
    // composed
    cSourceIOTransactionPacket generateListToSwitch(cSourceJsonParamApi requestedParams);
private:
    tSourceIoTransactionList generateRMSAndAngleUList();
    tSourceIoTransactionList generateRMSAndAngleIList();
    tSourceIoTransactionList generateSwitchPhasesList();
    tSourceIoTransactionList generateFrequencyList();
    tSourceIoTransactionList generateRegulationList();

    cSourceJsonStructureApi* m_jsonStructApi = nullptr;
    cSourceJsonParamApi m_paramsRequested;

    QByteArray m_ioPrefix;
};


class cSourceIoCmdHelper
{
public:
    static QByteArray formatDouble(double val, int preDigits, char digit, int postDigits);
};

#endif // CSOURCEIOTRANSACTIONGENERATOR_H
