#include "scpimodeldeviface.h"
#include "scpimodule.h"
#include "scpidelegateinterface.h"
#include <zscpi_response_definitions.h>
#include <zenuxdeviceinfo.h>

namespace SCPIMODULE
{

ScpiModelDevIface::ScpiModelDevIface(cSCPIModule *module) :
    m_pModule(module)
{
}

enum scpiinterfacecommands
{
    deviceinterfacecmd
};

void ScpiModelDevIface::setupScpi(cSCPIInterface *scpiInterface)
{
    cSCPIInterfaceDelegatePtr delegate = std::make_shared<ScpiDelegateInterface>(ScpiDelegateInterface::Params{"DEVICE", "IFACE", SCPI::isQuery, deviceinterfacecmd});
    scpiInterface->addSCPICommand(delegate);
    connect(delegate.get(), &ScpiDelegateInterface::signalExecuteSCPI, this, &ScpiModelDevIface::executeCmd);

    // for module integrity we also have to add this command to the scpi command list (exported at INF_ModuleInterface)
    m_pModule->scpiCommandList.append(new VfModuleMetaInfoContainer("", QString("DEVICE:IFACE"), SCPI::isQuery, ""));
}

void ScpiModelDevIface::executeCmd(cSCPIClient *client,
                                   int cmdCode,
                                   const QString &scpi,
                                   const ScpiTransactionId &scpiTransactionId)
{
    cSCPICommand cmd = scpi;
    switch (cmdCode)
    {
    case deviceinterfacecmd: {
        if (cmd.isQuery())
            client->handleCmdFinish(getDevIface(client), scpiTransactionId, cSCPIClient::LOG_SKIP /* dev:iface is huge */);
        else
            client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
        break;
    }

    }
}

QString ScpiModelDevIface::getDevIface(cSCPIClient *client)
{
    if (m_devIfaceCache.isEmpty()) {
        QMap<QString, QString> modelListBaseEntry({{"RELEASE", ZenuxDeviceInfo::getZenuxRelease()}});
        client->getScpiInterface()->exportSCPIModelXML(m_devIfaceCache, modelListBaseEntry);
    }
    return m_devIfaceCache;
}

}
