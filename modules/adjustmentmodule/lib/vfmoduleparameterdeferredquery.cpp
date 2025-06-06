#include "vfmoduleparameterdeferredquery.h"

VfModuleParameterDeferredQuery::VfModuleParameterDeferredQuery(int entityId,
                                                               VeinEvent::EventSystem *eventsystem,
                                                               QString name,
                                                               QString description,
                                                               QVariant initval,
                                                               bool deferredNotification) :
    VfModuleParameter(entityId,
                      eventsystem,
                      name,
                      description,
                      initval,
                      deferredNotification)
{
}

void VfModuleParameterDeferredQuery::veinTransaction(QUuid clientId,
                                                     QVariant newValue,
                                                     QVariant oldValue,
                                                     VeinComponent::ComponentData::Command vccmd)
{
    Q_UNUSED(oldValue)
    mClientIdList.append(clientId);
    if (vccmd == VeinComponent::ComponentData::Command::CCMD_FETCH)
        emit sigValueQuery(newValue); // deferred
    else
        handleNoFetchVeinTransaction(newValue);
}
