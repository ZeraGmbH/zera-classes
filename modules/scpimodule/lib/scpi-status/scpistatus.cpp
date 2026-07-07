#include "scpistatus.h"
#include "scpiclient.h"
#include "ieee488-2-definitions.h"
#include <scpi.h>

namespace SCPIMODULE
{

cSCPIStatus::cSCPIStatus(quint8 bitPositionToThrow) :
    m_bitPositionToThrow(bitPositionToThrow)
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
            if (ok) {
                status = regValue;
                client->handleCmdFinish(NullableString(), scpiTransactionId);
            }
            else
                emit sigEventErrorCmdFinish(NumericDataError, scpiTransactionId);
        }
        else
            emit sigEventErrorCmdFinish(CommandError, scpiTransactionId);
    }
}

void cSCPIStatus::readStatusReg(cSCPIClient *client, quint16 &status, const QString &input, const ScpiTransactionId &scpiTransactionId)
{
    cSCPICommand cmd = input;
    if (cmd.isQuery())
        client->handleCmdFinish(QString("%1").arg(status), scpiTransactionId);
    else
        emit sigEventErrorCmdFinish(CommandError, scpiTransactionId);
}

void cSCPIStatus::executeCmd(cSCPIClient* client,
                             SCPIStatusDefinitions::ScpiStatusCommands cmdCode,
                             const QString &scpi,
                             const ScpiTransactionId &scpiTransactionId)
{
    switch (cmdCode)
    {
    case SCPIStatusDefinitions::condition:
        readStatusReg(client, m_nCondition, scpi, scpiTransactionId);
        break;

    case SCPIStatusDefinitions::ptransition:
        readwriteStatusReg(client, m_nPTransition, scpi, scpiTransactionId);
        break;

    case SCPIStatusDefinitions::ntransition:
        readwriteStatusReg(client, m_nNTransition, scpi, scpiTransactionId);
        break;

    case SCPIStatusDefinitions::event:
        readStatusReg(client, m_nEvent, scpi, scpiTransactionId);
        break;

    case SCPIStatusDefinitions::enable:
        readwriteStatusReg(client, m_nEnable, scpi, scpiTransactionId);
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
            emit sigEvent(m_bitPositionToThrow, 1);
        else
            emit sigEvent(m_bitPositionToThrow, 0);
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
