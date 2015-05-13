#include <scpi.h>

#include "ieee4882interface.h"
#include "scpiinterfacedelegate.h"
#include "scpiinterface.h"


namespace SCPIMODULE
{


cIEEE4882Interface::cIEEE4882Interface(VeinPeer *peer, cSCPIInterface *iface)
    :cBaseInterface(peer, iface)
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

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*OPC"), SCPI::isQuery | SCPI::isCmd, operationComplete);
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*ESE"), SCPI::isQuery | SCPI::isCmd, eventstatusenable);
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*SRE"), SCPI::isQuery | SCPI::isCmd, servicerequestenable);
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*CLS"), SCPI::isCmd, clearstatus);
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*RST"), SCPI::isCmd, reset);
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*IDN"), SCPI::isQuery, identification);
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*ESR"), SCPI::isQuery, eventstatusregister);
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*STB"), SCPI::isQuery, statusbyte);
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    delegate = new cSCPIInterfaceDelegate(QString(""), QString("*TST"), SCPI::isQuery, selftest);
    m_IEEE4882DelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));



    return true;
}


void cIEEE4882Interface::executeCmd(cSCPIClient *client, int cmdCode, QString &sInput)
{
    cSCPICommand cmd = sInput;

    switch (cmdCode)
    {
    case operationComplete:
        break;
    case eventstatusenable:
        break;
    case servicerequestenable:
        break;
    case clearstatus:
        break;
    case reset:
        break;
    case identification:
        break;
    case eventstatusregister:
        break;
    case statusbyte:
        break;
    case selftest:
        break;
    }
}



}
