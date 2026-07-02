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

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString(""), QString("*OPC"), SCPI::isQuery | SCPI::isCmd, operationComplete, QString("OPeration Complete"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString(""), QString("*ESE"), SCPI::isQuery | SCPI::isCmdwP, eventstatusenable, QString("Event Status Enable"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString(""), QString("*SRE"), SCPI::isQuery | SCPI::isCmdwP, servicerequestenable, QString("Service Request Enable"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString(""), QString("*CLS"), SCPI::isCmd, clearstatus, QString("Clear Status"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString(""), QString("*RST"), SCPI::isCmd, reset, QString("Reset"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString(""), QString("*IDN"), SCPI::isQuery, identification, QString("Identification query"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString(""), QString("*ESR"), SCPI::isQuery, eventstatusregister, QString("Event Status Register"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString(""), QString("*STB"), SCPI::isQuery, statusbyte, QString("Status Byte query"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString(""), QString("*TST"), SCPI::isQuery, selftest, QString("Self-Test query"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString("SYSTEM"), QString("ERROR"), SCPI::isNode | SCPI::isQuery, read1error, QString("Queries one error of internal error list"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString("SYSTEM:ERROR"), QString("COUNT"), SCPI::isQuery, readerrorcount, QString("Queries the number of error messages of internal error list"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    delegate = std::make_shared<cSCPIInterfaceDelegate>(QString("SYSTEM:ERROR"), QString("ALL"), SCPI::isQuery, readallerrors, QString("Queries all error stored in internal error list"));
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupIEEE4882::executeCmd);

    return true;
}


void ScpiGroupIEEE4882::executeCmd(cSCPIClient *client, int cmdCode, const QString &sInput, const ScpiTransactionId &scpiTransactionId)
{
    cIEEE4882* pIEEE4882 = client->getIEEE4882();
    pIEEE4882->executeCmd(client, cmdCode, sInput, scpiTransactionId);
}


}
