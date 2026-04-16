#ifndef IOGROUPGENERATOREXTSERIAL_H
#define IOGROUPGENERATOREXTSERIAL_H

#include <QByteArray>
#include <QJsonObject>
#include "sourceactions.h"
#include "jsonstructapi.h"
#include "ioqueuegroup.h"

class IoGroupGeneratorExtSerial
{
public:
    explicit IoGroupGeneratorExtSerial(JsonStructApi jsonStructApi);
    ~IoGroupGeneratorExtSerial();

    // single
    tIoTransferList generateListForAction(SourceActionTypes::ActionTypes actionType);
    // composed
    IoQueueGroup::Ptr generateOnOffGroup(JsonParamApi requestedParams);
    IoQueueGroup::Ptr generateStatusPollGroup();
private:
    tIoTransferList generateRMSAndAngleUList();
    tIoTransferList generateRMSAndAngleIList();
    tIoTransferList generateSwitchPhasesList();
    tIoTransferList generateFrequencyList();
    tIoTransferList generateRegulationList();

    tIoTransferList generateQueryStatusList();
    tIoTransferList generateQueryActualList();

    JsonStructApi m_jsonStructApi;
    JsonParamApi m_paramsRequested;

    QByteArray m_ioPrefix;
};


class IoCmdFormatHelper
{
public:
    static QByteArray formatDouble(double val, int preDigits, char digit, int postDigits);
};


#endif // IOGROUPGENERATOREXTSERIAL_H
