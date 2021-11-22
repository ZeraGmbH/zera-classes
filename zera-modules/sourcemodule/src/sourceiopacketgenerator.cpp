#include "sourceiopacketgenerator.h"
#include "sourceactions.h"

void cSourceSingleOutIn::setActionType(cSourceActionTypes::ActionTypes actionType)
{
    m_actionType = actionType;
}

bool cSourceSingleOutIn::operator ==(const cSourceSingleOutIn &other)
{
    return  m_actionType == other.m_actionType &&
            m_responseType == other.m_responseType &&
            m_bytesSend == other.m_bytesSend &&
            m_bytesExpected == other.m_bytesExpected;
}


cSourceIoPacketGenerator::cSourceIoPacketGenerator(QJsonObject jsonParamsStructure) :
    m_jsonStructApi(new cSourceJsonStructApi(jsonParamsStructure)),
    m_ioPrefix(m_jsonStructApi->getIoPrefix())
{
}

cSourceIoPacketGenerator::~cSourceIoPacketGenerator()
{
    delete m_jsonStructApi;
}

cSourceCommandPacket cSourceIoPacketGenerator::generateOnOffPacket(cSourceJsonParamApi requestedParams)
{
    m_paramsRequested = requestedParams;
    tSourceActionTypeList actionsTypeList = cSourceActionGenerator::generateSwitchActions(requestedParams);
    cSourceCommandPacket commandPack;
    commandPack.m_commandType = COMMAND_SWITCH;
    commandPack.m_errorBehavior = BEHAVE_STOP_ON_ERROR;
    for(auto &actionType : actionsTypeList) {
        commandPack.m_outInList.append(generateListForAction(actionType));
    }
    return commandPack;
}

cSourceCommandPacket cSourceIoPacketGenerator::generateStatusPollPacket()
{
    cSourceCommandPacket commandPack;
    commandPack.m_commandType = COMMAND_STATE_POLL;
    commandPack.m_errorBehavior = BEHAVE_CONTINUE_ON_ERROR;
    tSourceActionTypeList actionsTypeList = cSourceActionGenerator::generatePeriodicActions();
    for(auto &actionType : actionsTypeList) {
        commandPack.m_outInList.append(generateListForAction(actionType));
    }
    return commandPack;
}

tSourceOutInList cSourceIoPacketGenerator::generateListForAction(cSourceActionTypes::ActionTypes actionType)
{
    tSourceOutInList outInList;
    switch(actionType) {
    case cSourceActionTypes::SET_RMS:
        outInList.append(generateRMSAndAngleUList());
        outInList.append(generateRMSAndAngleIList());
        break;
    case cSourceActionTypes::SET_ANGLE:
        // On ZERA this is combined with RMS
        break;
    case cSourceActionTypes::SET_FREQUENCY:
        outInList.append(generateFrequencyList());
        break;
    case cSourceActionTypes::SET_HARMONICS:
        // TODO
        break;
    case cSourceActionTypes::SET_REGULATOR:
        outInList.append(generateRegulationList());
        break;
    case cSourceActionTypes::SWITCH_PHASES:
        outInList.append(generateSwitchPhasesList());
        break;
    default:
        break;
    }
    for(auto &outIn : outInList) {
        outIn.m_actionType = actionType;
    }
    return outInList;
}

tSourceOutInList cSourceIoPacketGenerator::generateRMSAndAngleUList()
{
    tSourceOutInList outInList;
    QByteArray bytesSend;

    double rmsU[3], angleU[3] = {0.0, 0.0, 0.0};
    for(int phase=0; phase<3; phase++) {
        if(phase < m_jsonStructApi->getCountUPhases()) {
            rmsU[phase] = m_paramsRequested.getRms(true, phase);
            angleU[phase] = m_paramsRequested.getAngle(true, phase);
        }
    }

    bytesSend = m_ioPrefix + "UP";
    // e-funct off for now
    bytesSend.append('A');
    // do not switch off voltage at dosage
    bytesSend.append('E');
    for(int idx=0; idx<3; idx++) {
        bytesSend.append('R'+idx);
        bytesSend += cSourceIoCmdHelper::formatDouble(rmsU[idx], 3, '.', 3);
        bytesSend += cSourceIoCmdHelper::formatDouble(angleU[idx], 3, '.', 2);
    }
    bytesSend.append('\r');
    QByteArray expectedResponse = m_ioPrefix + "OKUP\r";
    outInList.append(cSourceSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse));
    return outInList;
}

tSourceOutInList cSourceIoPacketGenerator::generateRMSAndAngleIList()
{
    tSourceOutInList outInList;
    QByteArray bytesSend;

    double rmsI[3], angleI[3] = {0.0, 0.0, 0.0};
    for(int phase=0; phase<3; phase++) {
        if(phase < m_jsonStructApi->getCountIPhases()) {
            rmsI[phase] = m_paramsRequested.getRms(false, phase);
            angleI[phase] = m_paramsRequested.getAngle(false, phase);
        }
    }

    bytesSend = m_ioPrefix + "IP";
    // e-funct off for now
    bytesSend.append('A');
    // dimension always Amps for now
    bytesSend.append('A');
    for(int idx=0; idx<3; idx++) {
        bytesSend.append('R'+idx);
        bytesSend += cSourceIoCmdHelper::formatDouble(rmsI[idx], 3, '.', 3);
        bytesSend += cSourceIoCmdHelper::formatDouble(angleI[idx], 3, '.', 2);
    }
    bytesSend.append('\r');
    QByteArray expectedResponse = m_ioPrefix + "OKIP\r";
    outInList.append(cSourceSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse));

    return outInList;
}

tSourceOutInList cSourceIoPacketGenerator::generateSwitchPhasesList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "UI";
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
        bytesSend.append(bPhaseOn ? "E" : "A");
    }
    // current
    for(int phase=0; phase<3; phase++) {
        bool phaseAvail = phase < phaseCountI;
        bPhaseOn = false;
        if(globalOn && phaseAvail) {
            bPhaseOn = m_paramsRequested.getOn(false, phase);
        }
        bytesSend.append(bPhaseOn ? "P" : "A");
    }
    // aux u
    bPhaseOn = false;
    if(globalOn && phaseCountU>3) {
        bPhaseOn = m_paramsRequested.getOn(true, 3);
    }
    bytesSend.append(bPhaseOn ? "E" : "A");
    // aux i
    bPhaseOn = false;
    if(globalOn && phaseCountI>3) {
        bPhaseOn = m_paramsRequested.getOn(false, 3);
    }
    bytesSend.append(bPhaseOn ? "E" : "A");
    // relative comparison - off for now
    bytesSend.append("A");

    bytesSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKUI\r";
    return tSourceOutInList() << cSourceSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse);
}

tSourceOutInList cSourceIoPacketGenerator::generateFrequencyList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "FR";
    bool quartzVar = m_paramsRequested.getFreqVarOn();
    if(quartzVar) {
        bytesSend += cSourceIoCmdHelper::formatDouble(m_paramsRequested.getFreqVal(), 2, '.', 2);
    }
    else {
        // for now we support 50Hz sync only
        bytesSend += cSourceIoCmdHelper::formatDouble(0.0, 2, '.', 2);
    }
    bytesSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKFR\r";
    return tSourceOutInList() << cSourceSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse);
}

tSourceOutInList cSourceIoPacketGenerator::generateRegulationList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "RE";

    // for now we pin regulation to on
    bytesSend.append("1");

    bytesSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKRE\r";
    return tSourceOutInList() << cSourceSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse);
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
