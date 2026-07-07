#include "scpigroupdeviface.h"
#include "scpimodule.h"
#include "scpiinterfacedelegate.h"
#include <zscpi_response_definitions.h>
#include <zenuxdeviceinfo.h>

namespace SCPIMODULE
{

ScpiGroupDevIface::ScpiGroupDevIface(cSCPIModule *module, cSCPIInterface *iface) :
    ScpiGroupBase(iface),
    m_pModule(module)
{
}

enum scpiinterfacecommands
{
    deviceinterfacecmd
};

bool ScpiGroupDevIface::setupScpi()
{
    cSCPIInterfaceDelegatePtr delegate = std::make_shared<cSCPIInterfaceDelegate>("DEVICE", "IFACE", SCPI::isQuery, deviceinterfacecmd);
    m_pSCPIInterface->addSCPICommand(delegate);
    connect(delegate.get(), &cSCPIInterfaceDelegate::signalExecuteSCPI, this, &ScpiGroupDevIface::executeCmd);

    // for module integrity we also have to add this command to the scpi command list (exported at INF_ModuleInterface)
    m_pModule->scpiCommandList.append(new VfModuleMetaInfoContainer("", QString("DEVICE:IFACE"), SCPI::isQuery, ""));
    return true;
}

void ScpiGroupDevIface::executeCmd(cSCPIClient *client,
                                   int cmdCode,
                                   const QString &scpi,
                                   const ScpiTransactionId &scpiTransactionId)
{
    cSCPICommand cmd = scpi;
    switch (cmdCode)
    {
    case deviceinterfacecmd: {
        if (cmd.isQuery())
            client->handleCmdFinish(getDevIface(), scpiTransactionId, cSCPIClient::LOG_SKIP /* dev:iface is huge */);
        else
            client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
        break;
    }

    }
}

QString ScpiGroupDevIface::getDevIface()
{
    if (m_devIfaceCache.isEmpty()) {
        QMap<QString, QString> modelListBaseEntry({{"RELEASE", ZenuxDeviceInfo::getZenuxRelease()}});
        m_pSCPIInterface->exportSCPIModelXML(m_devIfaceCache, modelListBaseEntry);
    }
    return m_devIfaceCache;
}

}
