#include <scpi.h>

#include "scpiclient.h"
#include "statusinterface.h"
#include "scpiinterface.h"
#include "scpistatus.h"
#include "scpistatusdelegate.h"

namespace SCPIMODULE
{

cStatusInterface::cStatusInterface(cSCPIModule *module, cSCPIInterface *iface)
    :cBaseInterface(module, iface)
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
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:QUESTIONABLE"), QString("PTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ptransition, SCPIStatusSystem::questionable);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:QUESTIONABLE"), QString("NTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ntransition, SCPIStatusSystem::questionable);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:QUESTIONABLE"), QString("EVENT"), SCPI::isQuery, SCPIStatusCmd::event, SCPIStatusSystem::questionable);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:QUESTIONABLE"), QString("ENABLE"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::enable, SCPIStatusSystem::questionable);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    // our operation status interface

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("CONDITION"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("PTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ptransition, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("NTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ntransition, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("EVENT"), SCPI::isQuery, SCPIStatusCmd::event, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION"), QString("ENABLE"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::enable, SCPIStatusSystem::operation);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    // our operation status interface

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("CONDITION"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("PTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ptransition, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("NTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ntransition, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("EVENT"), SCPI::isQuery, SCPIStatusCmd::event, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    delegate = new cSCPIStatusDelegate(QString("STATUS:OPERATION:MEASURE"), QString("ENABLE"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::enable, SCPIStatusSystem::operationmeasure);
    m_scpiStatusDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIStatusDelegate::signalExecuteSCPI, this, &cStatusInterface::executeCmd);

    return true;
}


void cStatusInterface::executeCmd(cSCPIClient *client, int cmdCode, int statIndex, const QString &sInput)
{
    QString answer;
    cSCPIStatus* status;

    answer="";
    status = client->getSCPIStatus(statIndex);
    status->executeCmd(client, cmdCode, sInput);
}



}

