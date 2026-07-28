#include "scpimodelieee4882.h"
#include "ieee488-2-definitions.h"
#include "scpiclient.h"
#include "scpidelegateinterface.h"
#include "scpiinterface.h"

namespace SCPIMODULE
{

void ScpiModelIEEE4882::setupScpi(cSCPIInterface *scpiInterface)
{
    cSCPIInterfaceDelegatePtr delegate;

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"", "*OPC",
                                                                                     SCPI::isQuery | SCPI::isCmd,
                                                                                     operationComplete,
                                                                                     "OPeration Complete"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"", "*ESE",
                                                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                                                     eventstatusenable,
                                                                                     "Event Status Enable"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"", "*SRE",
                                                                                     SCPI::isQuery | SCPI::isCmdwP,
                                                                                     servicerequestenable,
                                                                                     "Service Request Enable"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"", "*CLS",
                                                                                     SCPI::isCmd,
                                                                                     clearstatus,
                                                                                     "Clear Status"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"", "*RST",
                                                                                     SCPI::isCmd,
                                                                                     reset,
                                                                                     "Reset"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"", "*IDN",
                                                                                     SCPI::isQuery,
                                                                                     identification,
                                                                                     "Identification query"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"", "*ESR",
                                                                                     SCPI::isQuery,
                                                                                     eventstatusregister,
                                                                                     "Event Status Register"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"", "*STB",
                                                                                     SCPI::isQuery,
                                                                                     statusbyte,
                                                                                     "Status Byte query:\n"
                                                                                     "Bit0: Unused - always 0\n"
                                                                                     "Bit1: Unused - always 0\n"
                                                                                     "Bit2: Error queue not empty - read further details by SYSTEM:ERROR queries\n"
                                                                                     "Bit3: Questionable (=non fatal errors / warnings) available\n"
                                                                                     "Bit4: Message available - Unused - always 0\n"
                                                                                     "Bit5: Event summary - set if enabled events in ESE are set in ESR\n"
                                                                                     "Bit6: Requested service (or MSS Master summary status)\n"
                                                                                     "Bit7: Operation status summary\n"
                                                                                    });
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"", "*TST",
                                                                                     SCPI::isQuery,
                                                                                     selftest,
                                                                                     "Self-Test query"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);


    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"SYSTEM", "ERROR",
                                                                                     SCPI::isNode | SCPI::isQuery,
                                                                                     read1error,
                                                                                     "Queries first error from error queue and removes it from queue"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"SYSTEM:ERROR", "COUNT",
                                                                                     SCPI::isQuery,
                                                                                     readerrorcount,
                                                                                     "Queries the number of error messages of internal error list"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);

    delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"SYSTEM:ERROR", "ALL",
                                                                                     SCPI::isQuery,
                                                                                     readallerrors,
                                                                                     "Queries all errors from error queue and removes them from queue"});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelIEEE4882::executeCmd);
}


void ScpiModelIEEE4882::executeCmd(cSCPIClient *client,
                                   int cmdCode,
                                   const QString &scpi,
                                   const ScpiTransactionId &scpiTransactionId)
{
    cIEEE4882* pIEEE4882 = client->getIEEE4882();
    pIEEE4882->executeCmd(client, cmdCode, scpi, scpiTransactionId);
}


}
