#include "sourceiotransactiongenerator.h"
#include "sourceactions.h"

cSourceIoTransactionGenerator::cSourceIoTransactionGenerator()
{
}

void cSourceIoTransactionGenerator::setParamsStructure(QJsonObject jsonParamsStructure)
{
    m_jsonParamsStructure = jsonParamsStructure;
    m_ioPrefix = m_jsonParamsStructure["IoPrefix"].toString().toLatin1();
    m_countVoltagePhases = m_jsonParamsStructure["UPhaseMax"].toInt();
    m_countCurrentPhases = m_jsonParamsStructure["IPhaseMax"].toInt();
}

tSourceIoTransactionList cSourceIoTransactionGenerator::generateIoTransactionList(QJsonObject requestedParamState)
{
    m_requestedParamState = requestedParamState;
    tSourceActionTypeList actionsTypeList = cSourceActionGenerator::GenerateLoadActionList(m_requestedParamState);
    tSourceIoTransactionList transactionList;
    for(auto &actionType : actionsTypeList) {
        transactionList.append(generateListForAction(actionType));
    }
    return transactionList;
}

tSourceIoTransactionList cSourceIoTransactionGenerator::generateListForAction(cSourceActionTypes::ActionTypes actionType)
{
    tSourceIoTransactionList transactionList;
    switch(actionType) {
    case cSourceActionTypes::SET_RMS:
        transactionList.append(generateRMSList());
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

tSourceIoTransactionList cSourceIoTransactionGenerator::generateRMSList()
{
    tSourceIoTransactionList transactionList;
    QByteArray dataSend;

    double rmsU[3], angleU[3], rmsI[3], angleI[3] = {0.0, 0.0, 0.0};
    for(int phase=1; phase<=3; phase++) {
        if(phase <= m_countVoltagePhases) {
            int idx = phase-1;
            rmsU[idx] = m_requestedParamState[QString("U%1").arg(phase)].toObject()["rms"].toDouble();
            angleU[idx] = m_requestedParamState[QString("U%1").arg(phase)].toObject()["angle"].toDouble();
            rmsI[idx] = m_requestedParamState[QString("I%1").arg(phase)].toObject()["rms"].toDouble();
            angleI[idx] = m_requestedParamState[QString("I%1").arg(phase)].toObject()["angle"].toDouble();
        }
    }

    // voltage
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

    // current
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
    expectedResponse = m_ioPrefix + "OKIP\r";
    transactionList.append(cSourceIoTransaction(EXPECT_DATA_SEQUENCE, dataSend, expectedResponse));

    return transactionList;
}

tSourceIoTransactionList cSourceIoTransactionGenerator::generateSwitchPhasesList()
{
    QByteArray dataSend;
    dataSend = m_ioPrefix + "UI";
    bool globalOn = m_requestedParamState["on"].toBool();
    bool bPhaseOn = false;
    // voltage
    for(int phase=1; phase<=3; phase++) {
        bool phaseAvail = phase <= m_countVoltagePhases;
        bPhaseOn = false;
        if(globalOn && phaseAvail) {
            bPhaseOn = m_requestedParamState[QString("U%1").arg(phase)].toObject()["on"].toBool();
        }
        dataSend.append(bPhaseOn ? "E" : "A");
    }
    // current
    for(int phase=1; phase<=3; phase++) {
        bool phaseAvail = phase <= m_countCurrentPhases;
        bPhaseOn = false;
        if(globalOn && phaseAvail) {
            bPhaseOn = m_requestedParamState[QString("I%1").arg(phase)].toObject()["on"].toBool();
        }
        dataSend.append(bPhaseOn ? "P" : "A");
    }
    // aux u
    bPhaseOn = false;
    if(globalOn && m_countVoltagePhases>3) {
        bPhaseOn = m_requestedParamState[QString("U%1").arg(4)].toObject()["on"].toBool();
    }
    dataSend.append(bPhaseOn ? "E" : "A");
    // aux i
    bPhaseOn = false;
    if(globalOn && m_countCurrentPhases>3) {
        bPhaseOn = m_requestedParamState[QString("I%1").arg(4)].toObject()["on"].toBool();
    }
    dataSend.append(bPhaseOn ? "E" : "A");
    // relative comparison - off for now
    dataSend.append("A");

    dataSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKUI\r";
    return tSourceIoTransactionList() << cSourceIoTransaction(EXPECT_DATA_SEQUENCE, dataSend, expectedResponse);
}

tSourceIoTransactionList cSourceIoTransactionGenerator::generateFrequencyList()
{
    QByteArray dataSend;
    dataSend = m_ioPrefix + "FR";
    bool quartzVar = m_requestedParamState["Frequency"].toObject()["type"] == "var";
    if(quartzVar) {
        double frequency = m_requestedParamState["Frequency"].toObject()["val"].toDouble();
        dataSend += cSourceIoCmdHelper::formatDouble(frequency, 2, '.', 2);
    }
    else {
        // for now we support 50Hz sync only
        dataSend += cSourceIoCmdHelper::formatDouble(0.0, 2, '.', 2);
    }
    dataSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKFR\r";
    return tSourceIoTransactionList() << cSourceIoTransaction(EXPECT_DATA_SEQUENCE, dataSend, expectedResponse);
}

tSourceIoTransactionList cSourceIoTransactionGenerator::generateRegulationList()
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
