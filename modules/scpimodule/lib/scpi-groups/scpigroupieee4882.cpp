#include "scpigroupieee4882.h"
#include "ieee488-2.h"
#include "scpiclient.h"
#include "scpiinterfacedelegate.h"
#include "scpiinterface.h"

namespace SCPIMODULE
{

ScpiGroupIEEE4882::ScpiGroupIEEE4882(cSCPIInterface* iface) :
    ScpiGroupBase(iface)
{
}

bool ScpiGroupIEEE4882::setupScpi()
{
    cSCPIInterfaceDelegatePtr delegate;

    delegate = std::make_shared<cSCPIInterfaceDelegate>("", "*OPC", SCPI::isQuery | SCPI::isCmd, operationComplete, "OPeration Complete");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("", "*ESE", SCPI::isQuery | SCPI::isCmdwP, eventstatusenable, "Event Status Enable");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("", "*SRE", SCPI::isQuery | SCPI::isCmdwP, servicerequestenable, "Service Request Enable");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("", "*CLS", SCPI::isCmd, clearstatus, "Clear Status");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("", "*RST", SCPI::isCmd, reset, "Reset");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("", "*IDN", SCPI::isQuery, identification, "Identification query");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("", "*ESR", SCPI::isQuery, eventstatusregister, "Event Status Register");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("", "*STB", SCPI::isQuery, statusbyte, "Status Byte query");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("", "*TST", SCPI::isQuery, selftest, "Self-Test query");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);


    delegate = std::make_shared<cSCPIInterfaceDelegate>("SYSTEM", "ERROR", SCPI::isNode | SCPI::isQuery, read1error, "Queries one error of internal error list");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("SYSTEM:ERROR", "COUNT", SCPI::isQuery, readerrorcount, "Queries the number of error messages of internal error list");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>("SYSTEM:ERROR", "ALL", SCPI::isQuery, readallerrors, "Queries all error stored in internal error list");
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    return true;
}


void ScpiGroupIEEE4882::executeCmd(cSCPIClient *client,
                                   int cmdCode,
                                   const QString &scpi,
                                   const ScpiTransactionId &scpiTransactionId)
{
    cIEEE4882* pIEEE4882 = client->getIEEE4882();
    pIEEE4882->executeCmd(client, cmdCode, scpi, scpiTransactionId);
}


}
