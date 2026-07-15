#include "scpimodelstatus.h"
#include "scpiclient.h"
#include "scpiinterface.h"
#include "scpistatus.h"
#include "scpidelegatestatus.h"

namespace SCPIMODULE
{

void ScpiModelStatus::setupScpi(cSCPIInterface *scpiInterface)
{
    cSCPIStatusDelegatePtr delegate;

    // our questionable status interface
    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:QUESTIONABLE", "CONDITION",
                                                                               SCPI::isQuery,
                                                                               SCPIStatusDefinitions::condition,
                                                                               SCPIStatusDefinitions::questionable});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:QUESTIONABLE", "PTRANSITION",
                                                                               SCPI::isQuery | SCPI::isCmdwP,
                                                                               SCPIStatusDefinitions::ptransition,
                                                                               SCPIStatusDefinitions::questionable});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:QUESTIONABLE", "NTRANSITION",
                                                                               SCPI::isQuery | SCPI::isCmdwP,
                                                                               SCPIStatusDefinitions::ntransition,
                                                                               SCPIStatusDefinitions::questionable});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:QUESTIONABLE", "EVENT",
                                                                               SCPI::isQuery,
                                                                               SCPIStatusDefinitions::event,
                                                                               SCPIStatusDefinitions::questionable});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:QUESTIONABLE", "ENABLE",
                                                                               SCPI::isQuery | SCPI::isCmdwP,
                                                                               SCPIStatusDefinitions::enable,
                                                                               SCPIStatusDefinitions::questionable});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    // our operation status interface

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION", "CONDITION",
                                                                               SCPI::isQuery,
                                                                               SCPIStatusDefinitions::condition,
                                                                               SCPIStatusDefinitions::operation});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION", "PTRANSITION",
                                                                               SCPI::isQuery | SCPI::isCmdwP,
                                                                               SCPIStatusDefinitions::ptransition,
                                                                               SCPIStatusDefinitions::operation});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION", "NTRANSITION",
                                                                               SCPI::isQuery | SCPI::isCmdwP,
                                                                               SCPIStatusDefinitions::ntransition,
                                                                               SCPIStatusDefinitions::operation});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION", "EVENT",
                                                                               SCPI::isQuery,
                                                                               SCPIStatusDefinitions::event,
                                                                               SCPIStatusDefinitions::operation});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION", "ENABLE",
                                                                               SCPI::isQuery | SCPI::isCmdwP,
                                                                               SCPIStatusDefinitions::enable,
                                                                               SCPIStatusDefinitions::operation});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    // our operation status interface

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION:MEASURE", "CONDITION",
                                                                               SCPI::isQuery,
                                                                               SCPIStatusDefinitions::condition,
                                                                               SCPIStatusDefinitions::operationmeasure});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION:MEASURE", "PTRANSITION",
                                                                               SCPI::isQuery | SCPI::isCmdwP,
                                                                               SCPIStatusDefinitions::ptransition,
                                                                               SCPIStatusDefinitions::operationmeasure});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION:MEASURE", "NTRANSITION",
                                                                               SCPI::isQuery | SCPI::isCmdwP,
                                                                               SCPIStatusDefinitions::ntransition,
                                                                               SCPIStatusDefinitions::operationmeasure});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION:MEASURE", "EVENT",
                                                                               SCPI::isQuery, SCPIStatusDefinitions::event,
                                                                               SCPIStatusDefinitions::operationmeasure});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);

    delegate = std::make_shared<ScpiDelegateStatus>(ScpiDelegateStatus::Params{"STATUS:OPERATION:MEASURE", "ENABLE",
                                                                               SCPI::isQuery | SCPI::isCmdwP,
                                                                               SCPIStatusDefinitions::enable,
                                                                               SCPIStatusDefinitions::operationmeasure});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateStatus::signalExecuteSCPI, this, &ScpiModelStatus::executeCmd);
}


void ScpiModelStatus::executeCmd(cSCPIClient *client,
                                 SCPIStatusDefinitions::ScpiStatusCommands cmdCode,
                                 SCPIStatusDefinitions::ScpiStatusSystems statIndex,
                                 const QString &scpi,
                                 const ScpiTransactionId &scpiTransactionId)
{
    cSCPIStatus* status = client->getSCPIStatus(statIndex);
    status->executeCmd(client, cmdCode, scpi, scpiTransactionId);
}

}

