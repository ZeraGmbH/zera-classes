#include <QJsonObject>
#include <QJsonArray>
#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>
#include <ve_commandevent.h>
#include <ve_eventsystem.h>
#include "vfmodulecomponent.h"

VfModuleComponent::VfModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString componentName, QString description, QVariant initval) :
    m_componentName(componentName),
    m_sDescription(description),
    m_vValue(initval),
    m_nEntityId(entityId),
    m_pEventSystem(eventsystem)
{
    sendNotification(VeinComponent::ComponentData::Command::CCMD_ADD);
}

void VfModuleComponent::exportMetaData(QJsonObject &jsObj)
{
    QJsonObject jsonObj;
    jsonObj.insert("Description", m_sDescription);
    if (!m_sChannelName.isEmpty()) {
        jsonObj.insert("ChannelName", m_sChannelName);
    }
    if (!m_sChannelUnit.isEmpty()) {
        jsonObj.insert("Unit", m_sChannelUnit);
    }
    jsObj.insert(m_componentName, jsonObj);
}

void VfModuleComponent::exportSCPIInfo(QJsonArray &jsArr)
{
    if (m_scpiInfo)
        m_scpiInfo->appendSCPIInfo(jsArr);
}



void VfModuleComponent::sendDummyNotificationForRangeChange()
{
    sendNotification(VeinComponent::ComponentData::Command::CCMD_SET);
}

void VfModuleComponent::setScpiInfo(const QString &model,
                                    const QString &cmd,
                                    int cmdTypeMask,
                                    SCPI::eSCPIEntryType entryType)
{
    m_scpiInfo = std::make_unique<VfModuleMetaInfoContainer>(model,
                                                         cmd,
                                                         cmdTypeMask,
                                                         m_componentName,
                                                         entryType);
}

void VfModuleComponent::setChannelName(QString name)
{
    m_sChannelName = name;
}

QString VfModuleComponent::getChannelName()
{
    return m_sChannelName;
}

void VfModuleComponent::setUnit(QString unit)
{
    m_sChannelUnit = unit;
}

QVariant VfModuleComponent::getValue()
{
    return m_vValue;
}

QString VfModuleComponent::getUnit()
{
    return m_sChannelUnit;
}

QString VfModuleComponent::getComponentName()
{
    return m_componentName;
}

void VfModuleComponent::setValue(QVariant value)
{
    m_vValue = value;
    sendNotification(VeinComponent::ComponentData::Command::CCMD_SET);
}

void VfModuleComponent::setError()
{
    VeinComponent::ComponentData *cData = new VeinComponent::ComponentData();
    cData->setEntityId(m_nEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
    cData->setComponentName(m_componentName);
    cData->setNewValue(m_vValue);

    VeinComponent::ErrorData *errData = new VeinComponent::ErrorData();
    errData->setEntityId(m_nEntityId);
    errData->setErrorDescription("Invalid parameter");
    errData->setOriginalData(cData);

    VeinEvent::CommandEvent *cEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, errData);
    QUuid id; // null id
    if (!mClientIdList.isEmpty()) {
        id = mClientIdList.takeFirst();
    }
    cEvent->setPeerId(id);
    emit m_pEventSystem->sigSendEvent(cEvent);
}

void VfModuleComponent::sendNotification(VeinComponent::ComponentData::Command vcmd)
{
    VeinComponent::ComponentData *cData = new VeinComponent::ComponentData();
    cData->setEntityId(m_nEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(vcmd);
    cData->setComponentName(m_componentName);
    cData->setNewValue(m_vValue);

    VeinEvent::CommandEvent *event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, cData);
    QUuid id; // null id
    if (!mClientIdList.isEmpty()) {
        id = mClientIdList.takeFirst();
    }
    event->setPeerId(id);
    emit m_pEventSystem->sigSendEvent(event);
}
