#include "sourceiotransactiongenerator.h"
#include "sourceactions.h"

cSourceSwitchIoTransactionGenerator::cSourceSwitchIoTransactionGenerator(QJsonObject jsonParamsStructure) :
    m_jsonStructApi(new cSourceJsonStructureApi(jsonParamsStructure)),
    m_ioPrefix(m_jsonStructApi->getIoPrefix())
{
}

cSourceSwitchIoTransactionGenerator::~cSourceSwitchIoTransactionGenerator()
{
    delete m_jsonStructApi;
}

tSourceIoTransactionList cSourceSwitchIoTransactionGenerator::generateIoTransactionList(cSourceJsonParamApi requestedParams)
{
    m_paramsRequested = requestedParams;
    tSourceActionTypeList actionsTypeList = cSourceActionGenerator::generateLoadActionList(requestedParams);
    tSourceIoTransactionList transactionList;
    for(auto &actionType : actionsTypeList) {
        transactionList.append(generateListForAction(actionType));
    }
    return transactionList;
}

tSourceIoTransactionList cSourceSwitchIoTransactionGenerator::generateListForAction(cSourceActionTypes::ActionTypes actionType)
{
    tSourceIoTransactionList transactionList;
    switch(actionType) {
    case cSourceActionTypes::SET_RMS:
        transactionList.append(generateRMSAndAngleUList());
        transactionList.append(generateRMSAndAngleIList());
        break;
    case cSourceActionTypes::SET_ANGLE:
        // On ZERA this is combined with RMS
        break;
    case cSourceActionTypes::SET_FREQUENCY:
        transactionList.append(generateFrequencyList());
        break;
    case cSourceActionTypes::SET_HARMONICS:
        // TODO
        break;
    case cSourceActionTypes::SET_REGULATOR:
        transactionList.append(generateRegulationList());
        break;
    case cSourceActionTypes::SWITCH_PHASES:
        transactionList.append(generateSwitchPhasesList());
        break;
    default:
        qCritical("Should not land in generateListForAction with %i - more coffee?", int(actionType));
        break;

    }
    return transactionList;
}

tSourceIoTransactionList cSourceSwitchIoTransactionGenerator::generateRMSAndAngleUList()
{
    tSourceIoTransactionList transactionList;
    QByteArray dataSend;

    double rmsU[3], angleU[3] = {0.0, 0.0, 0.0};
    for(int phase=0; phase<3; phase++) {
        if(phase < m_jsonStructApi->getCountUPhases()) {
            rmsU[phase] = m_paramsRequested.getRms(true, phase);
            angleU[phase] = m_paramsRequested.getAngle(true, phase);
        }
    }

    dataSend = m_ioPrefix + "UP";
    // e-funct off for now
    dataSend.append('A');
    // do not switch off voltage at dosage
    dataSend.append('E');
    for(int idx=0; idx<3; idx++) {
        dataSend.append('R'+idx);
        dataSend += cSourceIoCmdHelper::formatDouble(rmsU[idx], 3, '.', 3);
        dataSend += cSourceIoCmdHelper::formatDouble(angleU[idx], 3, '.', 2);
    }
    dataSend.append('\r');
    QByteArray expectedResponse = m_ioPrefix + "OKUP\r";
    transactionList.append(cSourceIoTransaction(EXPECT_DATA_SEQUENCE, dataSend, expectedResponse));
    return transactionList;
}

tSourceIoTransactionList cSourceSwitchIoTransactionGenerator::generateRMSAndAngleIList()
{
    tSourceIoTransactionList transactionList;
    QByteArray dataSend;

    double rmsI[3], angleI[3] = {0.0, 0.0, 0.0};
    for(int phase=0; phase<3; phase++) {
        if(phase < m_jsonStructApi->getCountIPhases()) {
            rmsI[phase] = m_paramsRequested.getRms(false, phase);
            angleI[phase] = m_paramsRequested.getAngle(false, phase);
        }
    }

    dataSend = m_ioPrefix + "IP";
    // e-funct off for now
    dataSend.append('A');
    // dimension always Amps for now
    dataSend.append('A');
    for(int idx=0; idx<3; idx++) {
        dataSend.append('R'+idx);
        dataSend += cSourceIoCmdHelper::formatDouble(rmsI[idx], 3, '.', 3);
        dataSend += cSourceIoCmdHelper::formatDouble(angleI[idx], 3, '.', 2);
    }
    dataSend.append('\r');
    QByteArray expectedResponse = m_ioPrefix + "OKIP\r";
    transactionList.append(cSourceIoTransaction(EXPECT_DATA_SEQUENCE, dataSend, expectedResponse));

    return transactionList;
}

tSourceIoTransactionList cSourceSwitchIoTransactionGenerator::generateSwitchPhasesList()
{
    QByteArray dataSend;
    dataSend = m_ioPrefix + "UI";
    bool globalOn = m_paramsRequested.getOn();
    bool bPhaseOn = false;
    int phaseCountU = m_jsonStructApi->getCountUPhases();
    int phaseCountI = m_jsonStructApi->getCountIPhases();
    // voltage
    for(int phase=0; phase<3; phase++) {
        bool phaseAvail = phase < phaseCountU;
        bPhaseOn = false;
        if(globalOn && phaseAvail) {
            bPhaseOn = m_paramsRequested.getOn(true, phase);
        }
        dataSend.append(bPhaseOn ? "E" : "A");
    }
    // current
    for(int phase=0; phase<3; phase++) {
        bool phaseAvail = phase < phaseCountI;
        bPhaseOn = false;
        if(globalOn && phaseAvail) {
            bPhaseOn = m_paramsRequested.getOn(false, phase);
        }
        dataSend.append(bPhaseOn ? "P" : "A");
    }
    // aux u
    bPhaseOn = false;
    if(globalOn && phaseCountU>3) {
        bPhaseOn = m_paramsRequested.getOn(true, 3);
    }
    dataSend.append(bPhaseOn ? "E" : "A");
    // aux i
    bPhaseOn = false;
    if(globalOn && phaseCountI>3) {
        bPhaseOn = m_paramsRequested.getOn(false, 3);
    }
    dataSend.append(bPhaseOn ? "E" : "A");
    // relative comparison - off for now
    dataSend.append("A");

    dataSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKUI\r";
    return tSourceIoTransactionList() << cSourceIoTransaction(EXPECT_DATA_SEQUENCE, dataSend, expectedResponse);
}

tSourceIoTransactionList cSourceSwitchIoTransactionGenerator::generateFrequencyList()
{
    QByteArray dataSend;
    dataSend = m_ioPrefix + "FR";
    bool quartzVar = m_paramsRequested.getFreqVarOn();
    if(quartzVar) {
        dataSend += cSourceIoCmdHelper::formatDouble(m_paramsRequested.getFreqVal(), 2, '.', 2);
    }
    else {
        // for now we support 50Hz sync only
        dataSend += cSourceIoCmdHelper::formatDouble(0.0, 2, '.', 2);
    }
    dataSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKFR\r";
    return tSourceIoTransactionList() << cSourceIoTransaction(EXPECT_DATA_SEQUENCE, dataSend, expectedResponse);
}

tSourceIoTransactionList cSourceSwitchIoTransactionGenerator::generateRegulationList()
{
    QByteArray dataSend;
    dataSend = m_ioPrefix + "RE";

    // for now we pin regulation to on
    dataSend.append("1");

    dataSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKRE\r";
    return tSourceIoTransactionList() << cSourceIoTransaction(EXPECT_DATA_SEQUENCE, dataSend, expectedResponse);
}

QByteArray cSourceIoCmdHelper::formatDouble(double val, int preDigits, char digit, int postDigits)
{
    QByteArray data;
    QString str, strFormat;
    strFormat = "%" + QString("%1").arg(preDigits) + digit + "0" + QString("%1").arg(postDigits) + "f";
    data = str.asprintf(qPrintable(strFormat), val).toLatin1();
    QByteArray leadPre(preDigits+postDigits, '0');
    data = (leadPre+data).right(preDigits + 1 + postDigits);
    return data;
}
