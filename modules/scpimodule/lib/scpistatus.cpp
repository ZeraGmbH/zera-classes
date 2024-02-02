#include <scpi.h>

#include "scpistatus.h"
#include "scpiclient.h"
#include "ieee488-2.h"

namespace SCPIMODULE
{


cSCPIStatus::cSCPIStatus(quint8 tothrow)
    :m_n2Throw(tothrow)
{
    // we have an all clear scpi conform status system
    m_nCondition = m_nPTransition = m_nNTransition = m_nEvent = m_nEnable = 0;
}


void cSCPIStatus::readwriteStatusReg(cSCPIClient *client, quint16 &status, QString input)
{
    cSCPICommand cmd;
    QString par;
    quint16 regValue;
    bool ok;

    cmd = input;
    if (cmd.isQuery())
        client->receiveAnswer(QString("%1").arg(status));
    else {
        if (cmd.isCommand(1))
        {
            par = cmd.getParam(0);
            regValue = par.toInt(&ok);
            if (ok)
                status = regValue;
            else
                emit eventError(NumericDataError);
        }
        else
            emit eventError(CommandError);
        emit client->commandAnswered(client);
    }
}


void cSCPIStatus::readStatusReg(cSCPIClient *client, quint16 &status, QString input)
{
    Q_UNUSED(client)
    cSCPICommand cmd;

    cmd = input;
    if (cmd.isQuery())
        client->receiveAnswer(QString("%1").arg(status));
    else {
        emit eventError(CommandError);
        emit client->commandAnswered(client);
    }
}


void cSCPIStatus::executeCmd(cSCPIClient* client, int cmdCode, const QString &sInput)
{
    switch (cmdCode)
    {
    case SCPIStatusCmd::condition:
        readStatusReg(client, m_nCondition, sInput);
        break;

    case SCPIStatusCmd::ptransition:
        readwriteStatusReg(client, m_nPTransition, sInput);
        break;

    case SCPIStatusCmd::ntransition:
        readwriteStatusReg(client, m_nNTransition, sInput);
        break;

    case SCPIStatusCmd::event:
        readStatusReg(client, m_nEvent, sInput);
        break;

    case SCPIStatusCmd::enable:
        readwriteStatusReg(client, m_nEnable, sInput);
        break;
    }
}


void cSCPIStatus::setCondition(quint16 condition)
{
    quint16 changedCondition;
    bool posTransition;
    changedCondition = m_nCondition ^ condition;
    m_nCondition = condition;

    posTransition = ((changedCondition & m_nCondition) != 0);
    m_nEvent = (changedCondition & ((m_nPTransition & m_nCondition) | (m_nNTransition & (~m_nCondition))));
    if ( (m_nEvent & m_nEnable) > 0 )
    {
        if (posTransition)
            emit event(m_n2Throw, 1);
        else
            emit event(m_n2Throw, 0);
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
