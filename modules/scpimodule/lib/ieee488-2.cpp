#include "ieee488-2.h"
#include "scpiclient.h"
#include <zenuxdeviceinfo.h>

namespace SCPIMODULE
{

scpiErrorType SCPIError[scpiLastError] = {  {0,(char*)"No error"},
                                            {-100,(char*)"Command error"},
                                            {-103,(char*)"Invalid separator"},
                                            {-108,(char*)"Parameter not allowed"},
                                            {-109,(char*)"Missing parameter"},
                                            {-113,(char*)"Undefined header"},
                                            {-120,(char*)"Numeric data error"},

                                            {-200,(char*)"Execution error"},
                                            {-203,(char*)"Command protected"},
                                            {-240,(char*)"Hardware error"},
                                            {-241,(char*)"Hardware missing"},
                                            {-257,(char*)"File name error"},

                                            {-300,(char*)"Device-specific error"},
                                            {-310,(char*)"System error"},
                                            {-350,(char*)"Queue overflow"},

                                            {-400,(char*)"Query error"},

                                            {-500,(char*)"Power on"} };


cIEEE4882::cIEEE4882(cSCPIClient *client, const QString &deviceFamilyFromConfig, VeinStorage::AbstractDatabase *storageDb) :
    m_pClient(client),
    m_deviceFamilyFromConfig(deviceFamilyFromConfig),
    m_serNoComponent(storageDb->getFutureComponent(1150, "PAR_SerialNr"))
{
    m_nSTB = m_nSRE = m_nESR = m_nESE = 0;
}


void cIEEE4882::addEventErrorWithResponse(int error, const ScpiTransactionId &scpiTransactionId)
{
    SetSTB(m_nSTB | (1 << STBeeQueueNotEmpty)); // we have something in our output queue -> so we set status byte
    if ( m_ErrEventQueue.count() == getErrorQueueLength() ) {
        m_ErrEventQueue.pop_back();
        m_ErrEventQueue.append(QueueOverflow);
    }
    else
        m_ErrEventQueue.append(error);
    m_pClient->handleCmdFinish(NullableString(), scpiTransactionId);
}

void cIEEE4882::executeCmd(cSCPIClient *client, int cmdCode, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    cSCPICommand cmd = scpi;
    switch (cmdCode)
    {
    case operationComplete:
        if (cmd.isCommand(0)) {
            // for the moment we only reset opcstate, means we only support sequential commands
            m_nOPCState = OCAS;
            SetnoOperFlag(true); // wir setzen ocas, setzen das nooperpending flag => setzen opc im sesr
            client->handleCmdFinish(NullableString(), scpiTransactionId);
        }
        else if (cmd.isQuery())
            client->handleCmdFinish(stringifyRegisterForOutput(client->operationComplete()), scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case eventstatusenable:
        if (cmd.isCommand(1)) {
            bool ok;
            quint8 par = cmd.getParam(0).toInt(&ok);
            if (ok) {
                SetESE(par);
                client->handleCmdFinish(NullableString(), scpiTransactionId);
            }
            else
                addEventErrorWithResponse(NumericDataError, scpiTransactionId);
        }
        else if (cmd.isQuery())
            client->handleCmdFinish(stringifyRegisterForOutput(m_nESE), scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case servicerequestenable:
        if (cmd.isCommand(1)) {
            bool ok;
            quint8 par = cmd.getParam(0).toInt(&ok);
            if (ok) {
                SetSRE(par);
                client->handleCmdFinish(NullableString(), scpiTransactionId);
            }
            else
                addEventErrorWithResponse(NumericDataError, scpiTransactionId);
        }
        else if (cmd.isQuery())
            client->handleCmdFinish(stringifyRegisterForOutput(m_nSRE), scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case clearstatus:
        if (cmd.isCommand(0)) {
            ClearStatus();
            client->handleCmdFinish(NullableString(), scpiTransactionId);
        }
        else if (cmd.isQuery())
            addEventErrorWithResponse(QueryError, scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case reset:
        if (cmd.isCommand(0)) {
            ResetDevice();
            client->handleCmdFinish(NullableString(), scpiTransactionId);
        }
        else if (cmd.isQuery())
            addEventErrorWithResponse(QueryError, scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case identification:
        if (cmd.isQuery())
            client->handleCmdFinish(getIdentification(), scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case eventstatusregister:
        if (cmd.isQuery())
            client->handleCmdFinish(stringifyRegisterForOutput(m_nESR), scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case statusbyte:
        if (cmd.isQuery())
            client->handleCmdFinish(stringifyRegisterForOutput(m_nSTB), scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case selftest:
        if (cmd.isQuery())
            client->handleCmdFinish(QString("1"), scpiTransactionId); // for the moment test is passed
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case read1error:
        if (cmd.isQuery())
            client->handleCmdFinish(popScpiErrorAndMakeString(), scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case readerrorcount:
        if (cmd.isQuery())
            client->handleCmdFinish(QString("%1").arg(m_ErrEventQueue.count()), scpiTransactionId);
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;

    case readallerrors:
        if (cmd.isQuery()) {
            int errCount = m_ErrEventQueue.count();
            if (errCount == 0)
                client->handleCmdFinish(popScpiErrorAndMakeString(), scpiTransactionId);
            else {
                QString errStr = popScpiErrorAndMakeString();
                for (int i = 1; i < errCount; i++)
                    errStr = errStr + ";" + popScpiErrorAndMakeString();
                client->handleCmdFinish(errStr, scpiTransactionId);
            }
        }
        else
            addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;
    }
}

int cIEEE4882::getErrorQueueLength()
{
    return 50;
}

void cIEEE4882::setStatusByte(quint8 stb, quint8)
{
    SetSTB(m_nSTB | (1 << stb)); // we ignore the value here, it's only important for scpi status systems
}

QString cIEEE4882::getIdentification()
{
    QString companyName QStringLiteral("ZERA GmbH Koenigswinter");
    QString releaseNr = ZenuxDeviceInfo::getZenuxRelease();
    if(releaseNr.isEmpty())
        releaseNr = QStringLiteral("unknokwn-release");
    QString serialNr = m_serNoComponent->getValue().toString();
    if(serialNr.isEmpty()) // was zera-setup2 completed??
        serialNr = QStringLiteral("unknown-serialno");
    return QString("%1, %2, %3, %4").arg(companyName, m_deviceFamilyFromConfig, serialNr, releaseNr);
}

QString cIEEE4882::stringifyRegisterForOutput(quint8 reg)
{
    return QString("+%1").arg(reg);
}

QString cIEEE4882::popScpiErrorAndMakeString()
{
    if ( m_ErrEventQueue.empty() )
        return QString("+%1,%2").arg(SCPIError[0].ErrNum).arg(SCPIError[0].ErrTxt);
    else {
        int e = m_ErrEventQueue.first(); // fifo
        m_ErrEventQueue.pop_front();
        if ( m_ErrEventQueue.empty() )
            SetSTB(m_nSTB & ~(1 << STBeeQueueNotEmpty)); // we reset this bit

        return QString("%1,%2").arg(SCPIError[e].ErrNum).arg(SCPIError[e].ErrTxt);
    }
}

void cIEEE4882::ResetDevice()
{
    m_nOPCState = OCIS; // bei rst auch opcidle
    m_nOPCQState = OQIS;
    SetnoOperFlag(true);
}


void cIEEE4882::ClearStatus()
{
    SetESR(0); // standard event status register = 0
    SetSTB(0); // status byte = 0

    emit setQuestionableCondition(0);
    emit setOperationCondition(0);
    emit setOperationMeasureCondition(0);

    ClearEventError(); // error event queue löschen
    m_nOPCState = OCIS; // opcidle
    m_nOPCQState = OQIS;
}

void cIEEE4882::ClearEventError()
{
    SetSTB(m_nSTB & ~(1 << STBmav)); // clear message avail bit in status byte
    m_ErrEventQueue.clear(); // clear our event error queue
}

void cIEEE4882::SetnoOperFlag(bool b)
{
    m_bnoOperationPendingFlag = b;
    if ( (b) && (m_nOPCState == OCAS)) {
        m_nOPCState = OCIS; // if we are in operation complete active state, we change into idle
        SetESR(m_nESR | SESROperationComplete);
    }
}

void cIEEE4882::SetSTB(quint8 b)
{
    m_nSTB = b;
    if (m_nSTB & m_nSRE & 0xBF) { // if bit is enabled
        m_nSTB |= (1 << STBrqs); // we set the request service bit in stb
        if (((m_nSTB & m_nSRE) & (1 << STBrqs)) != 0)
            // This look like an additional 'async' response => return with invalid id
            m_pClient->handleCmdFinish(QString("SRQ"), ScpiTransactionId());
    }
}

void cIEEE4882::SetSRE(quint8 b)
{
    m_nSRE = b;
    SetSTB(m_nSTB); // we set already set bits !? ... yes if there are bits sets which become enabled now....
}

void cIEEE4882::SetESR(quint8 b)
{
    m_nESR = b;
    if (m_nESR & m_nESE) // if enabled we set bit in status byte
        SetSTB(m_nSTB | 1 << STBesb);
}

void cIEEE4882::SetESE(quint8 b)
{
    m_nESE = b;
    SetESR(m_nESR);
}

}
