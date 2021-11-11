#ifndef CSOURCEIOTRANSACTIONGENERATOR_H
#define CSOURCEIOTRANSACTIONGENERATOR_H

#include <QByteArray>
#include <QJsonObject>
#include "sourceactions.h"

enum IoResponseType {
    WAIT_FOR_TIMEOUT = 0,
    EXPECT_DATA_SEQUENCE
};
class cSourceIoTransaction
{
public:
    cSourceIoTransaction();
    cSourceIoTransaction(IoResponseType ioResponseType,
                         QByteArray dataSend,
                         QByteArray dataExpectedResponse) :
        m_ioResponseType(ioResponseType),
        m_dataSend(dataSend),
        m_dataExpectedResponse(dataExpectedResponse)
    {}

    IoResponseType m_ioResponseType;
    QByteArray m_dataSend;
    QByteArray m_dataExpectedResponse;
};

typedef QList<cSourceIoTransaction> tSourceIoTransactionList;

class cSourceIoTransactionGenerator
{
public:
    cSourceIoTransactionGenerator();
    void setParamsStructure(QJsonObject jsonParamsStructure);
    tSourceIoTransactionList generateIoTransactionList(QJsonObject requestedParamState);

private:
    tSourceIoTransactionList generateListForAction(cSourceActionTypes::ActionTypes actionType);

    tSourceIoTransactionList generateRMSList();
    tSourceIoTransactionList generateSwitchPhasesList();
    tSourceIoTransactionList generateFrequencyList();
    tSourceIoTransactionList generateRegulationList();

    QJsonObject m_jsonParamsStructure;
    QJsonObject m_requestedParamState;
    QJsonObject m_lastSetParamState;

    QByteArray m_ioPrefix;
    int m_countVoltagePhases;
    int m_countCurrentPhases;
};


class cSourceIoCmdHelper
{
public:
    static QByteArray formatDouble(double val, int preDigits, char digit, int postDigits);
};

#endif // CSOURCEIOTRANSACTIONGENERATOR_H
