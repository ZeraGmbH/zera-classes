#include <QJsonObject>
#include <QDateTime>
#include <QTime>
#include <QString>

#include <vcmp_componentdata.h>
#include <ve_commandevent.h>
#include <ve_eventsystem.h>

#include "veinmoduleerrorcomponent.h"

cVeinModuleErrorComponent::cVeinModuleErrorComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString modulename)
    :m_nEntityId(entityId), m_pEventSystem(eventsystem), m_sName(name), m_sModuleName(modulename)
{
}


void cVeinModuleErrorComponent::setValue(QVariant value)
{
    QJsonObject jsObj;
    QString tf = "yyyy/MM/dd HH:mm:ss";

    jsObj.insert("ModuleName", m_sModuleName);
    jsObj.insert("Time", QDateTime::currentDateTime().toString(tf));
    jsObj.insert("Error", value.toString());

    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(0);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
    cData->setComponentName(m_sName);
    cData->setNewValue(jsObj);

    VeinEvent::CommandEvent *event;
    event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData);

    m_pEventSystem->sigSendEvent(event);

}

