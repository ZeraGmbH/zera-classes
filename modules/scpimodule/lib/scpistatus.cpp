#include "scpistatus.h"
#include "scpiclient.h"
#include <scpi.h>

namespace SCPIMODULE
{

cSCPIStatus::cSCPIStatus(quint8 tothrow) :
    m_n2Throw(tothrow)
{
    // we have an all clear scpi conform status system
    m_nCondition = m_nPTransition = m_nNTransition = m_nEvent = m_nEnable = 0;
}

void cSCPIStatus::readwriteStatusReg(cSCPIClient *client, quint16 &status, const QString &input, const ScpiTransactionId &scpiTransactionId)
{
    cSCPICommand cmd = input;
    if (cmd.isQuery())
        client->handleCmdFinish(QString("%1").arg(status), scpiTransactionId);
    else {
        if (cmd.isCommand(1)) {
            QString par = cmd.getParam(0);
            bool ok;
            quint16 regValue = par.toInt(&ok);
            if (ok)
                status = regValue;
            else
                emit sigEventError(NumericDataError);
        }
        else
            emit sigEventError(CommandError);
        client->handleCmdFinish(NullableString(), scpiTransactionId);
    }
}

void cSCPIStatus::readStatusReg(cSCPIClient *client, quint16 &status, const QString &input, const ScpiTransactionId &scpiTransactionId)
{
    cSCPICommand cmd = input;
    if (cmd.isQuery())
        client->handleCmdFinish(QString("%1").arg(status), scpiTransactionId);
    else {
        emit sigEventError(CommandError);
        client->handleCmdFinish(NullableString(), scpiTransactionId);
    }
}

void cSCPIStatus::executeCmd(cSCPIClient* client, int cmdCode, const QString &sInput, const ScpiTransactionId &scpiTransactionId)
{
    switch (cmdCode)
    {
    case SCPIStatusCmd::condition:
        readStatusReg(client, m_nCondition, sInput, scpiTransactionId);
        break;

    case SCPIStatusCmd::ptransition:
        readwriteStatusReg(client, m_nPTransition, sInput, scpiTransactionId);
        break;

    case SCPIStatusCmd::ntransition:
        readwriteStatusReg(client, m_nNTransition, sInput, scpiTransactionId);
        break;

    case SCPIStatusCmd::event:
        readStatusReg(client, m_nEvent, sInput, scpiTransactionId);
        break;

    case SCPIStatusCmd::enable:
        readwriteStatusReg(client, m_nEnable, sInput, scpiTransactionId);
        break;
    }
}

void cSCPIStatus::setCondition(quint16 condition)
{
    quint16 changedCondition = m_nCondition ^ condition;
    m_nCondition = condition;

    bool posTransition = ((changedCondition & m_nCondition) != 0);
    m_nEvent = (changedCondition & ((m_nPTransition & m_nCondition) | (m_nNTransition & (~m_nCondition))));
    if ( (m_nEvent & m_nEnable) > 0 ) {
        if (posTransition)
            emit sigEvent(m_n2Throw, 1);
        else
            emit sigEvent(m_n2Throw, 0);
    }
}

void cSCPIStatus::SetConditionBit(quint8 bitpos, quint8 val)
{
    if(val == 0)
        setCondition(m_nCondition & (~(1 << bitpos)));
    else
        setCondition(m_nCondition | (1 << bitpos));
}

}
