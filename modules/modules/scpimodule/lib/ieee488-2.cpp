#include <scpi.h>

#include "ieee488-2.h"
#include "scpiclient.h"
#include "scpimodule.h"
#include <sysinfo.h>

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


cIEEE4882::cIEEE4882(cSCPIClient *client, QString ident, quint16 errorqueuelen) :
    m_pClient(client),
    m_nQueueLen(errorqueuelen)
{
    m_nSTB = m_nSRE = m_nESR = m_nESE = 0;
    setIdentification(ident);
}


void cIEEE4882::executeCmd(cSCPIClient *client, int cmdCode, const QString &sInput)
{
    cSCPICommand cmd = sInput;
    quint8 par;
    int anzError, i;
    QString sError;
    bool ok;


    switch (cmdCode)
    {
    case operationComplete:
        if (cmd.isCommand(0))
        {
            // for the moment we only reset opcstate, means we only support sequential commands
            m_nOPCState = OCAS;
            SetnoOperFlag(true); // wir setzen ocas, setzen das nooperpending flag => setzen opc im sesr
        }
        else
            if (cmd.isQuery())
                client->receiveAnswer(RegOutput(client->operationComplete()));
            else
                AddEventError(CommandError);
        break;

    case eventstatusenable:
        if (cmd.isCommand(1))
        {
            par = cmd.getParam(0).toInt(&ok);
            if (ok)
                SetESE(par);
            else
                AddEventError(NumericDataError);
        }
        else
            if (cmd.isQuery())
                client->receiveAnswer(RegOutput(m_nESE));
            else
                AddEventError(CommandError);
        break;

    case servicerequestenable:
        if (cmd.isCommand(1))
        {

            par = cmd.getParam(0).toInt(&ok);
            if (ok)
                SetSRE(par);
            else
                AddEventError(NumericDataError);
        }
        else
            if (cmd.isQuery())
                client->receiveAnswer(RegOutput(m_nSRE));
            else
                AddEventError(CommandError);
        break;

    case clearstatus:
        if (cmd.isCommand(0))
        {
            ClearStatus();
        }
        else
            if (cmd.isQuery())
                AddEventError(QueryError);
            else
                AddEventError(CommandError);
        break;

    case reset:
        if (cmd.isCommand(0))
        {
            ResetDevice();
        }
        else
            if (cmd.isQuery())
                AddEventError(QueryError);
            else
                AddEventError(CommandError);
        break;

    case identification:
        if (cmd.isQuery())
            client->receiveAnswer(m_sIdentification);
        else
            AddEventError(CommandError);
        break;

    case eventstatusregister:
        if (cmd.isQuery())
            client->receiveAnswer(RegOutput(m_nESR));
        else
            AddEventError(CommandError);
        break;

    case statusbyte:
        if (cmd.isQuery())
            client->receiveAnswer(RegOutput(m_nSTB));
        else
            AddEventError(CommandError);
        break;

    case selftest:
        if (cmd.isQuery())
            client->receiveAnswer(QString("1")); // for the moment test is passed
        else
            AddEventError(CommandError);
        break;

    case read1error:
        if (cmd.isQuery())
            client->receiveAnswer(mGetScpiError());
        else
            AddEventError(CommandError);
        break;

    case readerrorcount:
        if (cmd.isQuery())
            client->receiveAnswer(QString("%1").arg(m_ErrEventQueue.count()));
        else
            AddEventError(CommandError);
        break;

    case readallerrors:
        if (cmd.isQuery())
        {
            anzError = m_ErrEventQueue.count();
            if (anzError == 0)
                client->receiveAnswer(mGetScpiError());
            else
            {
                sError = mGetScpiError();
                if (anzError > 1)
                    for (i = 1; i < anzError; i++)
                        sError = sError + ";" + mGetScpiError();
                client->receiveAnswer(sError);
            }
        }
        else
            AddEventError(CommandError);

        break;
    }

}


void cIEEE4882::AddEventError(int error)
{
    SetSTB(m_nSTB | (1 << STBeeQueueNotEmpty)); // we have something in our output queue -> so we set status byte
    if ( m_ErrEventQueue.count() == m_nQueueLen )
    {
        m_ErrEventQueue.pop_back();
        m_ErrEventQueue.append(QueueOverflow);
    }
    else
        m_ErrEventQueue.append(error);
}


void cIEEE4882::setStatusByte(quint8 stb, quint8)
{
    SetSTB(m_nSTB | (1 << stb)); // we ignore the value here, it's only important for scpi status systems
}


void cIEEE4882::setIdentification(QString ident)
{
    QStringList splitIdent = ident.split(',');

    QString companyName;
    if(splitIdent.size() >= 1) {
        companyName = splitIdent[0].simplified();
    }
    if(companyName.isEmpty()) {
        companyName = QStringLiteral("ZERA GmbH Koenigswinter");
    }

    QString model;
    if(splitIdent.size() >= 2) {
        model = splitIdent[1].simplified();
    }
    if(model.isEmpty()) {
        model = QStringLiteral("unknown");
    }

    QString releaseNr = SysInfo::getReleaseNr();
    if(releaseNr.isEmpty() && splitIdent.size() >= 3) { // fallback to xml-config
        releaseNr = splitIdent[2].simplified();
    }
    if(releaseNr.isEmpty()) {
        releaseNr = QStringLiteral("unknown");
    }

    QString serialNr = SysInfo::getSerialNr();
    if(serialNr.isEmpty()) { // was zera-setup2 completed??
        serialNr = QStringLiteral("unknown");
    }
    m_sIdentification = QString("%1, %2, %3, %4").arg(companyName).arg(model).arg(serialNr).arg(releaseNr);
}


QString cIEEE4882::RegOutput(quint8 reg)
{
    return QString("+%1").arg(reg);
}


QString cIEEE4882::mGetScpiError()
{
    if ( m_ErrEventQueue.empty() )
        return QString("+%1,%2").arg(SCPIError[0].ErrNum).arg(SCPIError[0].ErrTxt);
    else
    {
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
    if ( (b) && (m_nOPCState == OCAS))
    { // wenn operation complete active state
        m_nOPCState = OCIS; // if we are in operation complete active state, we change into idle
        SetESR(m_nESR | SESROperationComplete);
    }
}


void cIEEE4882::SetSTB(quint8 b)
{
    m_nSTB = b;
    if (m_nSTB & m_nSRE & 0xBF) // if bit is enabled
    {
        m_nSTB |= (1 << STBrqs); // we set the request service bit in stb
        if (((m_nSTB & m_nSRE) & (1 << STBrqs)) != 0)
            m_pClient->receiveAnswer("SRQ");
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
