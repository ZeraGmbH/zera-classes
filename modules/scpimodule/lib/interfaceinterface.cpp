#include "scpiclient.h"
#include "interfaceinterface.h"
#include "scpiinterface.h"
#include "scpiinfo.h"
#include "scpimodule.h"
#include "scpiinterfacedelegate.h"
#include <scpi.h>
#include <zscpi_response_definitions.h>
#include <sysinfo.h>

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
    scpiInfo = new cSCPIInfo("", QString("DEVICE:IFACE"), SCPI::isQuery, "", SCPI::isComponent);
    m_pModule->scpiCommandList.append(scpiInfo);
    return true;
}


void cInterfaceInterface::executeCmd(cSCPIClient *client, int cmdCode, const QString &sInput)
{
    cSCPICommand cmd = sInput;

    switch (cmdCode)
    {
    case deviceinterfacecmd:
    {
        if (cmd.isQuery())
        {
            QString xml;
            QMap<QString, QString> modelListBaseEntry({{"RELEASE", SysInfo::getReleaseNr()}});
            m_pSCPIInterface->exportSCPIModelXML(xml, modelListBaseEntry);
            client->receiveAnswer(xml);
        }
        else
            client->receiveStatus(ZSCPI::nak);
        break;
    }

    }

}

}
