#ifndef CSOURCEIOTRANSACTIONGENERATOR_H
#define CSOURCEIOTRANSACTIONGENERATOR_H

#include <QByteArray>
#include <QJsonObject>
#include "sourceactions.h"
#include "sourcejsonparamapi.h"

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

class cSourceSwitchIoTransactionGenerator
{
public:
    cSourceSwitchIoTransactionGenerator(QJsonObject jsonParamsStructure);
    ~cSourceSwitchIoTransactionGenerator();

    tSourceIoTransactionList generateIoTransactionList(cSourceJsonParamApi requestedParams);

private:
    tSourceIoTransactionList generateListForAction(cSourceActionTypes::ActionTypes actionType);

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
