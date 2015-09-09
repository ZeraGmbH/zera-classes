#include "basemeasmodule.h"
#include "modulevalidator.h"
#include "veinmodulecomponent.h"


cBaseMeasModule::cBaseMeasModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem *storagesystem, cBaseModuleConfiguration *modcfg, QObject *parent)
    :cBaseModule(modnr, proxy, entityId, storagesystem, modcfg, parent)
{
    m_pModuleValidator = new cModuleValidator(this);

    m_pModuleErrorComponent = new cVeinModuleComponent(m_nEntityId, m_pModuleValidator,
                                                       QString("ERR_Message"),
                                                       QString("Component forwards the run time errors"),
                                                       QVariant(QString("")));
    veinModuleComponentList.append(m_pModuleErrorComponent);
    m_pModuleInterfaceComponent = new cVeinModuleComponent(m_nEntityId, m_pModuleValidator,
                                                           QString("INF_ModuleInterface"),
                                                           QString("Component forwards the modules interface"),
                                                           QByteArray());
    veinModuleComponentList.append(m_pModuleInterfaceComponent);
}


