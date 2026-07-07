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
    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:QUESTIONABLE", "CONDITION",
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::condition,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:QUESTIONABLE", "PTRANSITION",
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ptransition,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:QUESTIONABLE", "NTRANSITION",
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ntransition,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:QUESTIONABLE", "EVENT",
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::event,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:QUESTIONABLE", "ENABLE",
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::enable,
                                                     SCPIStatusDefinitions::questionable);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    // our operation status interface

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION", "CONDITION",
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::condition,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION", "PTRANSITION",
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ptransition,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION", "NTRANSITION",
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ntransition,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION", "EVENT",
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::event,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION", "ENABLE",
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::enable,
                                                     SCPIStatusDefinitions::operation);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    // our operation status interface

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION:MEASURE", "CONDITION",
                                                     SCPI::isQuery,
                                                     SCPIStatusDefinitions::condition,
                                                     SCPIStatusDefinitions::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION:MEASURE", "PTRANSITION",
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ptransition,
                                                     SCPIStatusDefinitions::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION:MEASURE", "NTRANSITION",
                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                     SCPIStatusDefinitions::ntransition,
                                                     SCPIStatusDefinitions::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION:MEASURE", "EVENT",
                                                     SCPI::isQuery, SCPIStatusDefinitions::event,
                                                     SCPIStatusDefinitions::operationmeasure);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIStatusDelegate::signalExecuteSCPI, this, &ScpiGroupStatus::executeCmd);

    delegate = std::make_shared<cSCPIStatusDelegate>("STATUS:OPERATION:MEASURE", "ENABLE",
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

