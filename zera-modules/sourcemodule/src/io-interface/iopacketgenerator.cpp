#include "iopacketgenerator.h"
#include "sourceactions.h"
#include "iointerfacebase.h"

void IoSingleOutIn::setActionType(SourceActionTypes::ActionTypes actionType)
{
    m_actionType = actionType;
}

bool IoSingleOutIn::operator ==(const IoSingleOutIn &other)
{
    return  m_actionType == other.m_actionType &&
            m_responseType == other.m_responseType &&
            m_responseTimeoutMs == other.m_responseTimeoutMs &&
            m_bytesSend == other.m_bytesSend &&
            m_bytesExpected == other.m_bytesExpected;
}

IoPacketGenerator::IoPacketGenerator(QJsonObject jsonParamsStructure) :
    m_jsonStructApi(new JsonStructApi(jsonParamsStructure)),
    m_ioPrefix(m_jsonStructApi->getIoPrefix())
{
}

IoPacketGenerator::~IoPacketGenerator()
{
    delete m_jsonStructApi;
}

IoCommandPacket IoPacketGenerator::generateOnOffPacket(JsonParamApi requestedParams)
{
    m_paramsRequested = requestedParams;
    tSourceActionTypeList actionsTypeList = SourceActionGenerator::generateSwitchActions(requestedParams);
    IoCommandPacket commandPack;
    commandPack.m_commandType = COMMAND_SWITCH;
    commandPack.m_errorBehavior = BEHAVE_STOP_ON_ERROR;
    for(auto &actionType : actionsTypeList) {
        commandPack.m_outInList.append(generateListForAction(actionType));
    }
    return commandPack;
}

IoCommandPacket IoPacketGenerator::generateStatusPollPacket()
{
    IoCommandPacket commandPack;
    commandPack.m_commandType = COMMAND_STATE_POLL;
    commandPack.m_errorBehavior = BEHAVE_CONTINUE_ON_ERROR;
    tSourceActionTypeList actionsTypeList = SourceActionGenerator::generatePeriodicActions();
    for(auto &actionType : actionsTypeList) {
        commandPack.m_outInList.append(generateListForAction(actionType));
    }
    return commandPack;
}

tIoOutInList IoPacketGenerator::generateListForAction(SourceActionTypes::ActionTypes actionType)
{
    tIoOutInList outInList;
    switch(actionType) {
    case SourceActionTypes::SET_RMS:
        outInList.append(generateRMSAndAngleUList());
        outInList.append(generateRMSAndAngleIList());
        break;
    case SourceActionTypes::SET_ANGLE:
        // On ZERA this is combined with RMS
        break;
    case SourceActionTypes::SET_FREQUENCY:
        outInList.append(generateFrequencyList());
        break;
    case SourceActionTypes::SET_HARMONICS:
        // TODO
        break;
    case SourceActionTypes::SET_REGULATOR:
        outInList.append(generateRegulationList());
        break;
    case SourceActionTypes::SWITCH_PHASES:
        outInList.append(generateSwitchPhasesList());
        break;

    case SourceActionTypes::QUERY_STATUS:
        outInList.append(generateQueryStatusList());
        break;
    case SourceActionTypes::QUERY_ACTUAL:
        outInList.append(generateQueryActualList());
        break;

    default:
        break;
    }
    for(auto &outIn : outInList) {
        outIn.m_actionType = actionType;
        if(outIn.m_responseTimeoutMs == 0) {
            outIn.m_responseTimeoutMs = sourceDefaultTimeout;
        }
    }
    return outInList;
}

tIoOutInList IoPacketGenerator::generateRMSAndAngleUList()
{
    tIoOutInList outInList;
    QByteArray bytesSend;

    double rmsU[3], angleU[3] = {0.0, 0.0, 0.0};
    for(int phase=0; phase<3; phase++) {
        if(phase < m_jsonStructApi->getCountUPhases()) {
            rmsU[phase] = m_paramsRequested.getRms(phaseType::U, phase);
            angleU[phase] = m_paramsRequested.getAngle(phaseType::U, phase);
        }
    }

    bytesSend = m_ioPrefix + "UP";
    // e-funct off for now
    bytesSend.append('A');
    // do not switch off voltage at dosage
    bytesSend.append('E');
    for(int idx=0; idx<3; idx++) {
        bytesSend.append('R'+idx);
        bytesSend += IoCmdFormatHelper::formatDouble(rmsU[idx], 3, '.', 3);
        bytesSend += IoCmdFormatHelper::formatDouble(angleU[idx], 3, '.', 2);
    }
    bytesSend.append('\r');
    QByteArray expectedResponse = m_ioPrefix + "OKUP\r";
    outInList.append(IoSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse));
    return outInList;
}

tIoOutInList IoPacketGenerator::generateRMSAndAngleIList()
{
    tIoOutInList outInList;
    QByteArray bytesSend;

    double rmsI[3], angleI[3] = {0.0, 0.0, 0.0};
    for(int phase=0; phase<3; phase++) {
        if(phase < m_jsonStructApi->getCountIPhases()) {
            rmsI[phase] = m_paramsRequested.getRms(phaseType::I, phase);
            angleI[phase] = m_paramsRequested.getAngle(phaseType::I, phase);
        }
    }

    bytesSend = m_ioPrefix + "IP";
    // e-funct off for now
    bytesSend.append('A');
    // dimension always Amps for now
    bytesSend.append('A');
    for(int idx=0; idx<3; idx++) {
        bytesSend.append('R'+idx);
        bytesSend += IoCmdFormatHelper::formatDouble(rmsI[idx], 3, '.', 3);
        bytesSend += IoCmdFormatHelper::formatDouble(angleI[idx], 3, '.', 2);
    }
    bytesSend.append('\r');
    QByteArray expectedResponse = m_ioPrefix + "OKIP\r";
    outInList.append(IoSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse));

    return outInList;
}

tIoOutInList IoPacketGenerator::generateSwitchPhasesList()
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
            bPhaseOn = m_paramsRequested.getOn(phaseType::U, phase);
        }
        bytesSend.append(bPhaseOn ? "E" : "A");
    }
    // current
    for(int phase=0; phase<3; phase++) {
        bool phaseAvail = phase < phaseCountI;
        bPhaseOn = false;
        if(globalOn && phaseAvail) {
            bPhaseOn = m_paramsRequested.getOn(phaseType::I, phase);
        }
        bytesSend.append(bPhaseOn ? "P" : "A");
    }
    // aux u
    bPhaseOn = false;
    if(globalOn && phaseCountU>3) {
        bPhaseOn = m_paramsRequested.getOn(phaseType::U, 3);
    }
    bytesSend.append(bPhaseOn ? "E" : "A");
    // aux i
    bPhaseOn = false;
    if(globalOn && phaseCountI>3) {
        bPhaseOn = m_paramsRequested.getOn(phaseType::I, 3);
    }
    bytesSend.append(bPhaseOn ? "E" : "A");
    // relative comparison - off for now
    bytesSend.append("A");

    bytesSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKUI\r";
    int timeout = globalOn ? 15000 : 5000;
    return tIoOutInList() << IoSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse, timeout);
}

tIoOutInList IoPacketGenerator::generateFrequencyList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "FR";
    bool quartzVar = m_paramsRequested.getFreqVarOn();
    if(quartzVar) {
        bytesSend += IoCmdFormatHelper::formatDouble(m_paramsRequested.getFreqVal(), 2, '.', 2);
    }
    else {
        // for now we support 50Hz sync only
        bytesSend += IoCmdFormatHelper::formatDouble(0.0, 2, '.', 2);
    }
    bytesSend.append("\r");
    QByteArray expectedResponse = m_ioPrefix + "OKFR\r";
    return tIoOutInList() << IoSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse);
}

tIoOutInList IoPacketGenerator::generateRegulationList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "RE";
    // for now we pin regulation to on
    bytesSend.append("1");
    bytesSend.append("\r");

    QByteArray expectedResponse = m_ioPrefix + "OKRE\r";
    return tIoOutInList() << IoSingleOutIn(RESP_FULL_DATA_SEQUENCE, bytesSend, expectedResponse);
}

tIoOutInList IoPacketGenerator::generateQueryStatusList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "SM\r"; // error condition for now

    QByteArray expectedResponse = m_ioPrefix + "\r";
    return tIoOutInList() << IoSingleOutIn(RESP_PART_DATA_SEQUENCE, bytesSend, expectedResponse);
}

tIoOutInList IoPacketGenerator::generateQueryActualList()
{
    QByteArray bytesSend;
    bytesSend = "AME0;3\r";// This is single phase!!

    QByteArray expectedResponse = "\r";
    return tIoOutInList() << IoSingleOutIn(RESP_PART_DATA_SEQUENCE, bytesSend, expectedResponse);
}

QByteArray IoCmdFormatHelper::formatDouble(double val, int preDigits, char digit, int postDigits)
{
    QByteArray data;
    QString str, strFormat;
    strFormat = "%" + QString("%1").arg(preDigits) + digit + "0" + QString("%1").arg(postDigits) + "f";
    data = str.asprintf(qPrintable(strFormat), val).toLatin1();
    QByteArray leadPre(preDigits+postDigits, '0');
    data = (leadPre+data).right(preDigits + 1 + postDigits);
    return data;
}
