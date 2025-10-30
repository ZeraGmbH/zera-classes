#include "scpiclient.h"
#include "interfaceinterface.h"
#include "scpiinterface.h"
#include "scpimodule.h"
#include "scpiinterfacedelegate.h"
#include <scpi.h>
#include <zscpi_response_definitions.h>
#include <zenuxdeviceinfo.h>

namespace SCPIMODULE
{

cInterfaceInterface::cInterfaceInterface(cSCPIModule *module, cSCPIInterface *iface)
    :cBaseInterface(module, iface)
{
}

bool cInterfaceInterface::setupInterface()
{
    cSCPIInterfaceDelegatePtr delegate = std::make_shared<cSCPIInterfaceDelegate>(
        QString("DEVICE"), QString("IFACE"), SCPI::isQuery, deviceinterfacecmd);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &cInterfaceInterface::executeCmd);

    // for module integrity we also have to add this command to the scpi command list (exported at INF_ModuleInterface)
    m_pModule->scpiCommandList.append(new VfModuleMetaInfoContainer("", QString("DEVICE:IFACE"), SCPI::isQuery, ""));
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
            QMap<QString, QString> modelListBaseEntry({{"RELEASE", ZenuxDeviceInfo::getZenuxRelease()}});
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
