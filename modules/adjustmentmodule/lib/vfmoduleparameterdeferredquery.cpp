#include "vfmoduleparameterdeferredquery.h"

VfModuleParameterDeferredQuery::VfModuleParameterDeferredQuery(int entityId,
                                                               VeinEvent::EventSystem *eventsystem,
                                                               const QString &componentName,
                                                               const QString &description,
                                                               const QVariant &initval,
                                                               bool deferredNotification) :
    VfModuleParameter(entityId,
                      eventsystem,
                      componentName,
                      description,
                      initval,
                      deferredNotification)
{
}

void VfModuleParameterDeferredQuery::veinTransaction(const QUuid &clientId,
                                                     const QVariant &newValue,
                                                     const QVariant &oldValue,
                                                     VeinComponent::ComponentData::Command vccmd)
{
    Q_UNUSED(oldValue)
    m_clientIdList.append(clientId);
    if (vccmd == VeinComponent::ComponentData::Command::CCMD_FETCH)
        emit sigValueQuery(newValue); // deferred
    else
        handleNoFetchVeinTransaction(newValue);
}
