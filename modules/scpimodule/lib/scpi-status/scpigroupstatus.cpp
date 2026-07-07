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
    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("CONDITION"),
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::condition,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("PTRANSITION"),
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ptransition,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("NTRANSITION"),
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ntransition,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("EVENT"),
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::event,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:QUESTIONABLE"), QString("ENABLE"),
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::enable,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    // our operation status interface

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("CONDITION"),
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::condition,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("PTRANSITION"),
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ptransition,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("NTRANSITION"),
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ntransition,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("EVENT"),
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::event,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION"), QString("ENABLE"),
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::enable,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    // our operation status interface

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("CONDITION"),
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::condition,
                                                     SCPIStatusDefinitions::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("PTRANSITION"),
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ptransition,
                                                     SCPIStatusDefinitions::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("NTRANSITION"),
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ntransition,
                                                     SCPIStatusDefinitions::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("EVENT"),
                                                     SCPI::isQuery, SCPIStatusDefinitions::event,
                                                     SCPIStatusDefinitions::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>(QString("STATUS:OPERATION:MEASURE"), QString("ENABLE"),
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::enable,
                                                     SCPIStatusDefinitions::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    return true;
}


void ScpiGroupStatus::executeCmd(cSCPIClient *client,
                                 SCPIStatusDefinitions::ScpiStatusCommands cmdCode,
                                 SCPIStatusDefinitions::ScpiStatusSystems statIndex,
                                 const QString &scpi,
                                 const ScpiTransactionId &scpiTransactionId)
{
    cSCPIStatus* status = client->getSCPIStatus(statIndex);
    status->executeCmd(client, cmdCode, scpi, scpiTransactionId);
}

}

