#include <scpi.h>

#include "scpiclient.h"
#include "interfaceinterface.h"
#include "scpiinterface.h"
#include "scpiinterfacedelegate.h"

namespace SCPIMODULE
{

cInterfaceInterface::cInterfaceInterface(VeinPeer *peer, cSCPIInterface *iface)
    :cBaseInterface(peer, iface)
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
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    return true;
}


void cInterfaceInterface::executeCmd(cSCPIClient *client, int cmdCode, QString &sInput)
{
    cSCPICommand cmd = sInput;

    switch (cmdCode)
    {
    case deviceinterfacecmd:
    {
        if (cmd.isQuery())
        {
            QString xml;
            m_pSCPIInterface->getSCPICmdInterface()->exportSCPIModelXML(xml);
            client->receiveAnswer(xml);
        }
        else
            client->receiveAnswer(SCPI::scpiAnswer[SCPI::nak]);

        break;
    }

    }
}

}
