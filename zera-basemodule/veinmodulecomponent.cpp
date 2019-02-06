#include <QJsonObject>
#include <QJsonArray>

#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>

#include <ve_commandevent.h>
#include <ve_eventsystem.h>

#include "veinmodulecomponent.h"


cVeinModuleComponent::cVeinModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval)
    :m_nEntityId(entityId), m_pEventSystem(eventsystem), m_sName(name), m_sDescription(description), m_vValue(initval)
{
    sendNotification(VeinComponent::ComponentData::Command::CCMD_ADD);
}


cVeinModuleComponent::~cVeinModuleComponent()
{
    sendNotification(VeinComponent::ComponentData::Command::CCMD_REMOVE);
}


void cVeinModuleComponent::exportMetaData(QJsonObject &jsObj)
{
    QJsonObject jsonObj;

    jsonObj.insert("Description", m_sDescription);
    if (!m_sChannelName.isEmpty())
        jsonObj.insert("ChannelName", m_sChannelName);
    if (!m_sChannelUnit.isEmpty())
        jsonObj.insert("Unit", m_sChannelUnit);

    jsObj.insert(m_sName, jsonObj);
}


void cVeinModuleComponent::setChannelName(QString name)
{
    m_sChannelName = name;
}


QString cVeinModuleComponent::getChannelName()
{
    return m_sChannelName;
}


void cVeinModuleComponent::setUnit(QString unit)
{
    m_sChannelUnit = unit;
}


QVariant cVeinModuleComponent::getValue()
{
    return m_vValue;
}


QString cVeinModuleComponent::getUnit()
{
    return m_sChannelUnit;
}


QString cVeinModuleComponent::getName()
{
    return m_sName;
}


void cVeinModuleComponent::setValue(QVariant value)
{
    m_vValue = value;
    sendNotification(VeinComponent::ComponentData::Command::CCMD_SET);
}


void cVeinModuleComponent::setError()
{
    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(m_nEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
    cData->setComponentName(m_sName);
    cData->setNewValue(m_vValue);

    VeinComponent::ErrorData *errData;

    errData = new VeinComponent::ErrorData();

    errData->setEntityId(m_nEntityId);
    errData->setErrorDescription("Invalid parameter");
    errData->setOriginalData(cData);

    VeinEvent::CommandEvent *cEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, errData);
    QUuid id; // null id

    if (!mClientIdList.isEmpty())
        id = mClientIdList.takeFirst();
    cEvent->setPeerId(id);

    m_pEventSystem->sigSendEvent(cEvent);

}


void cVeinModuleComponent::sendNotification(VeinComponent::ComponentData::Command vcmd)
{
    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(m_nEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(vcmd);
    cData->setComponentName(m_sName);
    cData->setNewValue(m_vValue);

    VeinEvent::CommandEvent *event;
    event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, cData);
    QUuid id; // null id

    if (!mClientIdList.isEmpty())
        id = mClientIdList.takeFirst();
    event->setPeerId(id);

    m_pEventSystem->sigSendEvent(event);
}

