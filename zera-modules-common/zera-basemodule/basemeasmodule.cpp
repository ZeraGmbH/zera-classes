#include "basemeasmodule.h"

cBaseMeasModule::cBaseMeasModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem *storagesystem, std::shared_ptr<cBaseModuleConfiguration> modcfg, QObject *parent)
    :cBaseModule(modnr, proxy, entityId, storagesystem, modcfg, parent)
{
    m_pModuleValidator = new VfEventSytemModuleParam(entityId, storagesystem);
    m_pModuleEventSystem = m_pModuleValidator;
}
