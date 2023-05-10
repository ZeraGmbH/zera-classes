#include <scpi.h>

#include "ieee4882interface.h"
#include "ieee488-2.h"
#include "scpiclient.h"
#include "scpiinterfacedelegate.h"
#include "scpiinterface.h"


namespace SCPIMODULE
{


cIEEE4882Interface::cIEEE4882Interface(cSCPIModule* module, cSCPIInterface* iface)
    :cBaseInterface(module, iface)
{
}


cIEEE4882Interface::~cIEEE4882Interface()
{
    for (int i = 0; i < m_IEEE4882DelegateList.count(); i++ )
        delete m_IEEE4882DelegateList.at(i);
}


bool cIEEE4882Interface::setupInterface()
{
    cSCPIInterfaceDelegate* delegate;

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*OPC"), SCPI::isQuery | SCPI::isCmd, operationComplete, QString("OPeration Complete"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*ESE"), SCPI::isQuery | SCPI::isCmdwP, eventstatusenable, QString("Event Status Enable"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*SRE"), SCPI::isQuery | SCPI::isCmdwP, servicerequestenable, QString("Service Request Enable"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*CLS"), SCPI::isCmd, clearstatus, QString("Clear Status"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*RST"), SCPI::isCmd, reset, QString("Reset"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*IDN"), SCPI::isQuery, identification, QString("Identification query"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*ESR"), SCPI::isQuery, eventstatusregister, QString("Event Status Register"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*STB"), SCPI::isQuery, statusbyte, QString("Status Byte query"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*TST"), SCPI::isQuery, selftest, QString("Self-Test query"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString("SYSTEM"), QString("ERROR"), SCPI::isNode | SCPI::isQuery, read1error, QString("Queries one error of internal error list"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString("SYSTEM:ERROR"), QString("COUNT"), SCPI::isQuery, readerrorcount, QString("Queries the number of error messages of internal error list"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    delegate = new cSCPIInterfaceDelegate(QString("SYSTEM:ERROR"), QString("ALL"), SCPI::isQuery, readallerrors, QString("Queries all error stored in internal error list"));
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cIEEE4882Interface::executeCmd);

    return true;
}


void cIEEE4882Interface::executeCmd(cSCPIClient *client, int cmdCode, const QString &sInput)
{
    cIEEE4882* pIEEE4882 = client->getIEEE4882();
    pIEEE4882->executeCmd(client, cmdCode, sInput);
}


}
