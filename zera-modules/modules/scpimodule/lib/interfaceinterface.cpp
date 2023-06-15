#include <scpi.h>

#include "scpiclient.h"
#include "interfaceinterface.h"
#include "scpiinterface.h"
#include "scpiinfo.h"
#include "scpimodule.h"
#include "scpiinterfacedelegate.h"

namespace SCPIMODULE
{

cInterfaceInterface::cInterfaceInterface(cSCPIModule *module, cSCPIInterface *iface)
    :cBaseInterface(module, iface)
{
}


cInterfaceInterface::~cInterfaceInterface()
{
    for (int i = 0; i < m_scpiInterfaceDelegateList.count(); i++ )
        delete m_scpiInterfaceDelegateList.at(i);
}


bool cInterfaceInterface::setupInterface()
{
    cSCPIInterfaceDelegate* delegate;

    delegate = new cSCPIInterfaceDelegate(QString("DEVICE"), QString("IFACE"), SCPI::isQuery, deviceinterfacecmd);
    m_scpiInterfaceDelegateList.append(delegate);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate, &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cInterfaceInterface::executeCmd);

    // for module integrity we also have to add this command to the scpi command list (exported at INF_ModuleInterface
    cSCPIInfo *scpiInfo;
    scpiInfo = new cSCPIInfo("", QString("DEVICE:IFACE"), "2", "", "0", "");
    m_pModule->scpiCommandList.append(scpiInfo);
    return true;
}


void cInterfaceInterface::executeCmd(cSCPIClient *client, int cmdCode, const QString &sInput)
{
    cSCPICommand cmd = sInput;

    QMetaObject::Connection myConn = connect(this, &cInterfaceInterface::signalAnswer, client, &cSCPIClient::receiveAnswer);

    switch (cmdCode)
    {
    case deviceinterfacecmd:
    {
        if (cmd.isQuery())
        {
            QString xml;
            m_pSCPIInterface->exportSCPIModelXML(xml);
            emit signalAnswer(xml);
            //client->receiveAnswer(xml);
        }
        else
            emit signalStatus(SCPI::nak);
            //client->receiveStatus(SCPI::nak);
        break;
    }

    }

    disconnect(myConn);
}

}
