#include "basemeasmodule.h"
#include "modulevalidator.h"
#include "veinmodulecomponent.h"
#include "veinmodulemetadata.h"


cBaseMeasModule::cBaseMeasModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem *storagesystem, std::shared_ptr<cBaseModuleConfiguration> modcfg, QObject *parent)
    :cBaseModule(modnr, proxy, entityId, storagesystem, modcfg, parent)
{
    m_pModuleValidator = new ModuleValidator(entityId, storagesystem, &veinModuleRpcList);
    m_pModuleEventSystem = m_pModuleValidator;
}



