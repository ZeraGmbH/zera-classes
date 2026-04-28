#include "iogroupgeneratorextserial.h"
#include "sourceactions.h"
#include "sourcezerastatequeryevaluator.h"

IoGroupGeneratorExtSerial::IoGroupGeneratorExtSerial(JsonStructApi jsonStructApi) :
    m_jsonStructApi(jsonStructApi),
    m_ioPrefix(m_jsonStructApi.getIoPrefix())
{
}

IoGroupGeneratorExtSerial::~IoGroupGeneratorExtSerial()
{
}

IoQueueGroup::Ptr IoGroupGeneratorExtSerial::generateOnOffGroup(JsonParamApi requestedParams)
{
    m_loadpointRequestedLast = requestedParams;
    tSourceActionTypeList actionsTypeList = SourceActionGenerator::generateSwitchActions(requestedParams);
    IoQueueGroup::Ptr transferGroup = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    for(auto &actionType : actionsTypeList) {
        transferGroup->appendTransferList(generateListForAction(actionType));
    }
    return transferGroup;
}

IoQueueGroup::Ptr IoGroupGeneratorExtSerial::generateStatusPollGroup()
{
    IoQueueGroup::Ptr transferGroup = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::CONTINUE_ON_ERROR);
    tSourceActionTypeList actionsTypeList = SourceActionGenerator::generatePeriodicActions();
    for(auto &actionType : actionsTypeList) {
        transferGroup->appendTransferList(generateListForAction(actionType));
    }
    return transferGroup;
}

tIoTransferList IoGroupGeneratorExtSerial::generateListForAction(SourceActionTypes::ActionTypes actionType)
{
    tIoTransferList outInList;
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
        //outInList.append(generateQueryActualList());
        break;

    default:
        break;
    }

    return outInList;
}

tIoTransferList IoGroupGeneratorExtSerial::generateRMSAndAngleUList()
{
    tIoTransferList outInList;
    QByteArray bytesSend;

    double rmsU[3] = {0.0, 0.0, 0.0};
    double angleU[3] = {0.0, 0.0, 0.0};
    for(int phase=0; phase<3; phase++) {
        if(phase < m_jsonStructApi.getCountUPhases()) {
            rmsU[phase] = m_loadpointRequestedLast.getRms(phaseType::U, phase);
            angleU[phase] = m_loadpointRequestedLast.getAngle(phaseType::U, phase);
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
    QByteArray expectedResponseLead = m_ioPrefix + "OKUP";
    outInList.append(IoTransferDataSingle::Ptr::create(
                         bytesSend,
                         expectedResponseLead,
                         "\r"));
    return outInList;
}

tIoTransferList IoGroupGeneratorExtSerial::generateRMSAndAngleIList()
{
    tIoTransferList outInList;
    QByteArray bytesSend;

    double rmsI[3] = {0.0, 0.0, 0.0};
    double angleI[3] = {0.0, 0.0, 0.0};
    for(int phase=0; phase<3; phase++) {
        if(phase < m_jsonStructApi.getCountIPhases()) {
            rmsI[phase] = m_loadpointRequestedLast.getRms(phaseType::I, phase);
            angleI[phase] = m_loadpointRequestedLast.getAngle(phaseType::I, phase);
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
    QByteArray expectedResponseLead = m_ioPrefix + "OKIP";
    outInList.append(IoTransferDataSingle::Ptr::create(
                         bytesSend,
                         expectedResponseLead,
                         "\r"));

    return outInList;
}

tIoTransferList IoGroupGeneratorExtSerial::generateSwitchPhasesList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "UI";
    bool globalOn = m_loadpointRequestedLast.getOn();
    bool bPhaseOn = false;
    int phaseCountU = m_jsonStructApi.getCountUPhases();
    int phaseCountI = m_jsonStructApi.getCountIPhases();
    // voltage
    for(int phase=0; phase<3; phase++) {
        bool phaseAvail = phase < phaseCountU;
        bPhaseOn = false;
        if(globalOn && phaseAvail) {
            bPhaseOn = m_loadpointRequestedLast.getOn(phaseType::U, phase);
        }
        bytesSend.append(bPhaseOn ? "E" : "A");
    }
    // current
    for(int phase=0; phase<3; phase++) {
        bool phaseAvail = phase < phaseCountI;
        bPhaseOn = false;
        if(globalOn && phaseAvail) {
            bPhaseOn = m_loadpointRequestedLast.getOn(phaseType::I, phase);
        }
        bytesSend.append(bPhaseOn ? "P" : "A");
    }
    // aux u
    bPhaseOn = false;
    if(globalOn && phaseCountU>3) {
        bPhaseOn = m_loadpointRequestedLast.getOn(phaseType::U, 3);
    }
    bytesSend.append(bPhaseOn ? "E" : "A");
    // aux i
    bPhaseOn = false;
    if(globalOn && phaseCountI>3) {
        bPhaseOn = m_loadpointRequestedLast.getOn(phaseType::I, 3);
    }
    bytesSend.append(bPhaseOn ? "E" : "A");
    // relative comparison - off for now
    bytesSend.append("A");

    bytesSend.append("\r");
    QByteArray expectedResponseLead = m_ioPrefix + "OKUI";
    int timeout = globalOn ? 15000 : 5000;
    return tIoTransferList() << IoTransferDataSingle::Ptr::create(
                                    bytesSend,
                                    expectedResponseLead,
                                    "\r",
                                    timeout);
}

tIoTransferList IoGroupGeneratorExtSerial::generateFrequencyList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "FR";
    bool quartzVar = m_loadpointRequestedLast.getFreqVarOn();
    if(quartzVar) {
        bytesSend += IoCmdFormatHelper::formatDouble(m_loadpointRequestedLast.getFreqVal(), 2, '.', 2);
    }
    else {
        // for now we support 50Hz sync only
        bytesSend += IoCmdFormatHelper::formatDouble(0.0, 2, '.', 2);
    }
    bytesSend.append("\r");
    QByteArray expectedResponseLead = m_ioPrefix + "OKFR";
    tIoTransferList outInList;
    outInList.append(IoTransferDataSingle::Ptr::create(
                         bytesSend,
                         expectedResponseLead,
                         "\r"));
    return outInList;
}

tIoTransferList IoGroupGeneratorExtSerial::generateRegulationList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "RE";
    // for now we pin regulation to on
    bytesSend.append("1");
    bytesSend.append("\r");

    QByteArray expectedResponseLead = m_ioPrefix + "OKRE";
    tIoTransferList outInList;
    outInList.append(IoTransferDataSingle::Ptr::create(
                         bytesSend,
                         expectedResponseLead,
                         "\r"));
    return outInList;
}

tIoTransferList IoGroupGeneratorExtSerial::generateQueryStatusList()
{
    QByteArray bytesSend;
    bytesSend = m_ioPrefix + "SM\r";
    QByteArray expectedResponseLead = m_ioPrefix + "SM";
    tIoTransferList outInList;
    IoTransferDataSingle::Ptr singleIo = IoTransferDataSingle::Ptr::create(
                bytesSend,
                expectedResponseLead,
                "\r",
                2500);
    singleIo->setCustomQueryContentEvaluator(IIoQueryContentEvaluator::Ptr(new SourceZeraStateQueryEvaluator));
    outInList.append(singleIo);
    return outInList;
}

tIoTransferList IoGroupGeneratorExtSerial::generateQueryActualList()
{
    QByteArray bytesSend;
    bytesSend = "AME0;3\r";// This is single phase!!

    QByteArray expectedResponseLead = "AME";
    tIoTransferList outInList;
    outInList.append(IoTransferDataSingle::Ptr::create(
                         bytesSend,
                         expectedResponseLead,
                         "\r"));
    return outInList;
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
