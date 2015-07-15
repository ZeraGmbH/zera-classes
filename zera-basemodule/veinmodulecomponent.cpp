#include <QJsonObject>
#include <QJsonArray>

#include <vcmp_componentdata.h>
#include <ve_commandevent.h>
#include <ve_eventsystem.h>

#include "veinmodulecomponent.h"


cVeinModuleComponent::cVeinModuleComponent(int entityId, VeinEvent::EventSystem* eventsystem, QString name, QString description, QVariant initval)
    :m_nEntityId(entityId), m_pEventSystem(eventsystem), m_sName(name), m_sDescription(description), m_vValue(initval)
{
    sendNotification(VeinComponent::ComponentData::Command::CCMD_ADD);
}


void cVeinModuleComponent::exportMetaData(QJsonArray &jsArr)
{
    QJsonObject jsonObj;

    exportComponentMetaData(jsonObj);
    jsArr.append(jsonObj);
}


void cVeinModuleComponent::setChannelName(QString name)
{
    m_sChannelName = name;
}


void cVeinModuleComponent::setChannelUnit(QString unit)
{
    m_sChannelUnit = unit;
}


void cVeinModuleComponent::setValue(QVariant value)
{
    m_vValue = value;
    sendNotification(VeinComponent::ComponentData::Command::CCMD_SET);
}


void cVeinModuleComponent::exportComponentMetaData(QJsonObject jsObj)
{
    jsObj.insert("Name", m_sName);
    jsObj.insert("DES", m_sDescription);
    if (!m_sChannelName.isEmpty())
        jsObj.insert("ChannelName", m_sChannelName);
    if (!m_sChannelUnit.isEmpty())
        jsObj.insert("ChannelName", m_sChannelUnit);

}


void cVeinModuleComponent::sendNotification(VeinComponent::ComponentData::Command vcmd)
{
    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(m_nEntityId);
    cData->setCommand(vcmd);
    cData->setComponentName(m_sName);
    cData->setNewValue(m_vValue);

    VeinEvent::CommandEvent *event;
    event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData);

    m_pEventSystem->sigSendEvent(event);
}

