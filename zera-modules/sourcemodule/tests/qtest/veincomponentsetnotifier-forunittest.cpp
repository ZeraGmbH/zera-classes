#include "veincomponentsetnotifier-forunittest.h"
#include <vcmp_entitydata.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <vcmp_entitydata.h>

using VeinComponent::EntityData;
using VeinEvent::CommandEvent;
using VeinEvent::EventData;
using VeinComponent::ComponentData;

VeinComponentSetNotifier::VeinComponentSetNotifier(int entityId)
{
    m_eventHandler.addSubsystem(this);
    m_eventHandler.addSubsystem(&m_storageHash);
    createEntity(entityId);
}

void VeinComponentSetNotifier::addComponentToNotify(QString componentName, const QVariant *componentValue)
{
    m_hashComponentValuesListening[componentName] = componentValue;
}

void VeinComponentSetNotifier::createEntity(int entityId)
{
    VeinComponent::EntityData *entityData = new VeinComponent::EntityData();
    entityData->setCommand(VeinComponent::EntityData::Command::ECMD_ADD);
    entityData->setEventOrigin(VeinComponent::EntityData::EventOrigin::EO_LOCAL);
    entityData->setEventTarget(VeinComponent::EntityData::EventTarget::ET_ALL);
    entityData->setEntityId(entityId);
    emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, entityData));
}

bool VeinComponentSetNotifier::processEvent(QEvent *t_event)
{
    if(t_event->type() == VeinEvent::CommandEvent::eventType()) {
        VeinEvent::CommandEvent *cmdEvent = static_cast<VeinEvent::CommandEvent *>(t_event);
        if(cmdEvent != 0) {
            EventData *evData = cmdEvent->eventData();
            if(evData->type() == ComponentData::dataType()) {
                ComponentData *cData = static_cast<ComponentData *>(evData);
                if(cData->eventCommand() == ComponentData::Command::CCMD_SET) {
                    QString componentName = cData->componentName();
                    auto iter = m_hashComponentValuesListening.find(componentName);
                    if(iter != m_hashComponentValuesListening.end()) {
                        int entityId = evData->entityId();
                        QVariant oldValue = m_storageHash.getStoredValue(entityId, componentName);
                        const QVariant newValue = *iter.value();
                        if(oldValue != newValue) {
                            emit sigComponentChanged(componentName, newValue);
                        }
                    }
                }
            }
        }
    }
    return false; // why is processEvent returning bool - it is ignored anyway?
}

