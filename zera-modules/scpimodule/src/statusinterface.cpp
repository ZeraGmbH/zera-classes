#include <scpi.h>

#include "scpiclient.h"
#include "statusinterface.h"
#include "scpiinterface.h"
#include "scpistatus.h"
#include "scpistatusdelegate.h"

namespace SCPIMODULE
{

cStatusInterface::cStatusInterface(VeinPeer *peer, cSCPIInterface *iface)
    :cBaseInterface(peer, iface)
{
}


cStatusInterface::~cStatusInterface()
{
    for (int i = 0; i < m_scpiStatusDelegateList.count(); i++ )
        delete m_scpiStatusDelegateList.at(i);
}


bool cStatusInterface::setupInterface()
{
    cSCPIStatusDelegate *delegate;

    // our questionable status interface
    delegate = new cSCPIStatusDelegate(QString("STATUS:QUESTIONABLE"), QString("CONDITION"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::questionable);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:QUESTIONABLE"), QString("PTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ptransition, SCPIStatusSystem::questionable);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:QUESTIONABLE"), QString("NTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ntransition, SCPIStatusSystem::questionable);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:QUESTIONABLE"), QString("EVENT"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::questionable);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:QUESTIONABLE"), QString("ENABLE"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::enable, SCPIStatusSystem::questionable);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    // our operation status interface

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("CONDITION"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("PTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ptransition, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("NTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ntransition, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("EVENT"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("ENABLE"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::enable, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    // our operation status interface

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("CONDITION"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("PTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ptransition, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("NTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ntransition, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("EVENT"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("ENABLE"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::enable, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*,int,int,QString&)), this, SLOT(executeCmd(cSCPIClient*,int,int,QString&)));

    return true;
}


QString cStatusInterface::readwriteStatusReg(quint16 &status, QString input)
{
    cSCPICommand cmd;
    QString par;
    quint16 regValue;
    bool ok;

    cmd = input;
    if (cmd.isQuery())
        return QString("%1").arg(status);
    else
        if (cmd.isCommand(1))
        {
            par = cmd.getParam(0);
            regValue = par.toInt(&ok);
            if (ok)
            {
                status = regValue;
                return SCPI::scpiAnswer[SCPI::ack];
            }
            else
                return SCPI::scpiAnswer[SCPI::errval];
        }
        else
            return SCPI::scpiAnswer[SCPI::nak];
}


QString cStatusInterface::readStatusReg(quint16 &status, QString input)
{
    cSCPICommand cmd;

    cmd = input;
    if (cmd.isQuery())
        return QString("%1").arg(status);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


void cStatusInterface::executeCmd(cSCPIClient *client, int cmdCode, int statIndex, QString &sInput)
{
    QString answer;
    cSCPIStatus* status;

    answer="";
    status = client->getSCPIStatus(statIndex);

    switch (cmdCode)
    {
    case SCPIStatusCmd::condition:
        answer = readStatusReg(status->m_nCondition, sInput);
        break;

    case SCPIStatusCmd::ptransition:
        answer = readwriteStatusReg(status->m_nPTransition, sInput);
        break;

    case SCPIStatusCmd::ntransition:
        answer = readwriteStatusReg(status->m_nNTransition, sInput);
        break;

    case SCPIStatusCmd::event:
        answer = readStatusReg(status->m_nEvent, sInput);
        break;

    case SCPIStatusCmd::enable:
        answer = readwriteStatusReg(status->m_nEnable, sInput);
        break;
    }

    client->receiveAnswer(answer);
}



}

