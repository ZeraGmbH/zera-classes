#include "basemeasmodule.h"

cBaseMeasModule::cBaseMeasModule(quint8 modnr, int entityId, VeinEvent::StorageSystem *storagesystem, std::shared_ptr<cBaseModuleConfiguration> modcfg, bool demo) :
    cBaseModule(modnr, entityId, storagesystem, modcfg, demo)
{
    m_pModuleValidator = new VfEventSytemModuleParam(entityId, storagesystem);
    m_pModuleEventSystem = m_pModuleValidator;
}
