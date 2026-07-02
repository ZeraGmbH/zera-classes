#include "scpigroupstatus.h"
#include "scpiclient.h"
#include "scpiinterface.h"
#include "scpistatus.h"
#include "scpistatusdelegate.h"

namespace SCPIMODULE
{

ScpiGroupStatus::ScpiGroupStatus(cSCPIInterface *iface) :
    ScpiGroupBase(iface)
{
}

bool ScpiGroupStatus::setupScpi()
{
    cSCPIStatusDelegatePtr delegate;

    // our questionable status interface
    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("CONDITION"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("PTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ptransition, SCPIStatusSystem::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("NTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ntransition, SCPIStatusSystem::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("EVENT"), SCPI::isQuery, SCPIStatusCmd::event, SCPIStatusSystem::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("ENABLE"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::enable, SCPIStatusSystem::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    // our operation status interface

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("CONDITION"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("PTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ptransition, SCPIStatusSystem::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("NTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ntransition, SCPIStatusSystem::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("EVENT"), SCPI::isQuery, SCPIStatusCmd::event, SCPIStatusSystem::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("ENABLE"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::enable, SCPIStatusSystem::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    // our operation status interface

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("CONDITION"), SCPI::isQuery, SCPIStatusCmd::condition, SCPIStatusSystem::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("PTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ptransition, SCPIStatusSystem::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("NTRANSITION"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::ntransition, SCPIStatusSystem::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("EVENT"), SCPI::isQuery, SCPIStatusCmd::event, SCPIStatusSystem::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("ENABLE"), SCPI::isQuery | SCPI::isCmdwP, SCPIStatusCmd::enable, SCPIStatusSystem::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    return true;
}


void ScpiGroupStatus::executeCmd(cSCPIClient *client, int cmdCode, int statIndex, const QString &sInput, const ScpiTransactionId &scpiTransactionId)
{
    QString answer;
    cSCPIStatus* status;

    answer="";
    status = client->getSCPIStatus(statIndex);
    status->executeCmd(client, cmdCode, sInput, scpiTransactionId);
}



}

