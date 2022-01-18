#ifndef IOGROUPGENERATOR_H
#define IOGROUPGENERATOR_H

#include <QByteArray>
#include <QJsonObject>
#include "sourceactions.h"
#include "json/jsonstructapi.h"
#include "io-device/iomultipletransfergroup.h"

class IoGroupGenerator
{
public:
    IoGroupGenerator(JsonStructApi jsonStructApi);
    ~IoGroupGenerator();

    // single
    tIoTransferList generateListForAction(SourceActionTypes::ActionTypes actionType);
    // composed
    IoMultipleTransferGroup generateOnOffGroup(JsonParamApi requestedParams);
    IoMultipleTransferGroup generateStatusPollGroup();
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


#endif // IOGROUPGENERATOR_H
