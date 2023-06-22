#include "modulemanagercontroller.h"

#include <ve_eventdata.h>
#include <ve_commandevent.h>
#include <ve_storagesystem.h>
#include <vcmp_componentdata.h>
#include <vcmp_entitydata.h>
#include <vcmp_introspectiondata.h>

#include <QNetworkInterface>
#include <QHostAddress>

#include <QJsonArray>
#include <QDateTime>

//constexpr definition, see: https://stackoverflow.com/questions/8016780/undefined-reference-to-static-constexpr-char
constexpr QLatin1String ModuleManagerController::s_entityName;
constexpr QLatin1String ModuleManagerController::s_entityNameComponentName;
constexpr QLatin1String ModuleManagerController::s_entitiesComponentName;
constexpr QLatin1String ModuleManagerController::s_sessionComponentName;
constexpr QLatin1String ModuleManagerController::s_sessionsAvailableComponentName;
constexpr QLatin1String ModuleManagerController::s_notificationMessagesComponentName;
constexpr QLatin1String ModuleManagerController::s_loggedComponentsComponentName;
constexpr QLatin1String ModuleManagerController::s_modulesPausedComponentName;
constexpr QLatin1String ModuleManagerController::s_serverIpComponentName;

ModuleManagerController::ModuleManagerController(QObject *t_parent) :
    VeinEvent::EventSystem(t_parent)
{
}

constexpr int ModuleManagerController::getEntityId()
{
    return s_entityId;
}

VeinEvent::StorageSystem *ModuleManagerController::getStorageSystem() const
{
    return m_storageSystem;
}

void ModuleManagerController::setStorage(VeinEvent::StorageSystem *t_storageSystem)
{
    m_storageSystem = t_storageSystem;
}

bool ModuleManagerController::processEvent(QEvent *t_event)
{
    bool retVal = false;


    if(t_event->type() == VeinEvent::CommandEvent::eventType())
    {
        bool validated=false;
        VeinEvent::CommandEvent *cEvent = nullptr;
        cEvent = static_cast<VeinEvent::CommandEvent *>(t_event);
        Q_ASSERT(cEvent != nullptr);
        if(cEvent->eventSubtype() != VeinEvent::CommandEvent::EventSubtype::NOTIFICATION) //we do not need to process notifications
        {
            if(cEvent->eventData()->type() == VeinComponent::IntrospectionData::dataType()) //introspection requests are always valid
            {
                validated = true;
            }
            else if(cEvent->eventData()->type() == VeinComponent::EntityData::dataType()) //validate subscription requests
            {
                VeinComponent::EntityData *eData=nullptr;
                eData = static_cast<VeinComponent::EntityData *>(cEvent->eventData());
                Q_ASSERT(eData != nullptr);
                if(eData->eventCommand() == VeinComponent::EntityData::Command::ECMD_SUBSCRIBE
                        || eData->eventCommand() == VeinComponent::EntityData::Command::ECMD_UNSUBSCRIBE) /// @todo maybe add roles/views later
                {
                    validated = true;
                }
            }
            else if(cEvent->eventData()->type() == VeinComponent::ComponentData::dataType())
            {
                VeinComponent::ComponentData *cData=nullptr;
                cData = static_cast<VeinComponent::ComponentData *>(cEvent->eventData());
                Q_ASSERT(cData != nullptr);

                //validate fetch requests
                if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_FETCH) /// @todo maybe add roles/views later
                {
                    validated = true;
                }
                else if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_SET && //validate set event for _System.Session
                        cData->entityId() == s_entityId)
                {
                    if(cData->componentName() == ModuleManagerController::s_sessionComponentName)
                    {
                        m_currentSession=cData->newValue().toString();
                        if(m_sessionReady == true)
                        {
                            emit sigChangeSession(cData->newValue().toString());
                            m_sessionReady = false;
                        }
                        t_event->accept();
                    }
                    else if(cData->componentName() == ModuleManagerController::s_notificationMessagesComponentName)
                    {
                        handleNotificationMessage(cData->newValue().toJsonObject());
                        t_event->accept();
                    }
                    else if(cData->componentName() == ModuleManagerController::s_loggedComponentsComponentName)
                    {
                        validated=true;
                    }
                    else if(cData->componentName() == ModuleManagerController::s_modulesPausedComponentName)
                    {
                        validated=true;
                        setModulesPaused(cData->newValue().toBool());
                    }
                }
            }
        }

        if(validated == true)
        {
            ///@todo @bug remove inconsistent behavior by sending a new event instead of rewriting the current event
            retVal = true;
            cEvent->setEventSubtype(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION);
            cEvent->eventData()->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL); //the validated answer is authored from the system that runs the validator (aka. this system)
            cEvent->eventData()->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL); //inform all users (may or may not result in network messages)
        }
    }
    return retVal;
}





void ModuleManagerController::initializeEntity(const QString &t_sessionPath, const QStringList &t_sessionList)
{
    if(m_storageSystem!=nullptr)
    {
        m_sessionReady=true;
        m_currentSession=t_sessionPath;
        m_availableSessions=t_sessionList;

        VeinComponent::ComponentData *initData=nullptr;
        VeinEvent::CommandEvent *initEvent = nullptr;


        initData = new VeinComponent::ComponentData();
        initData->setEntityId(s_entityId);
        initData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
        initData->setComponentName(ModuleManagerController::s_entitiesComponentName);
        initData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        initData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
        qDebug() << "ENTITIES" << m_storageSystem->getEntityList() << QVariant::fromValue<QList<int> >(m_storageSystem->getEntityList()).value<QList<int> >();
        initData->setNewValue(QVariant::fromValue<QList<int> >(m_storageSystem->getEntityList()));

        initEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, initData);
        emit sigSendEvent(initEvent);
        initEvent=nullptr;


        initData = new VeinComponent::ComponentData();
        initData->setEntityId(s_entityId);
        initData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
        initData->setComponentName(ModuleManagerController::s_sessionComponentName);
        initData->setNewValue(QVariant(m_currentSession));
        initData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        initData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
        initEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, initData);
        emit sigSendEvent(initEvent);
        initEvent=nullptr;


        initData = new VeinComponent::ComponentData();
        initData->setEntityId(s_entityId);
        initData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
        initData->setComponentName(ModuleManagerController::s_sessionsAvailableComponentName);
        initData->setNewValue(QVariant(m_availableSessions));
        initData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        initData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
        initEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, initData);
        emit sigSendEvent(initEvent);
    }
    else
    {
        qCritical() << "[ModuleManagerController] StorageSystem required to call initializeEntities";
    }
}

void ModuleManagerController::initOnce()
{
    if(m_initDone == false)
    {
        VeinComponent::EntityData *systemData = new VeinComponent::EntityData();
        systemData->setCommand(VeinComponent::EntityData::Command::ECMD_ADD);
        systemData->setEntityId(s_entityId);

        emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, systemData));

        QVariantList ipAdressList;
        for (const QHostAddress &address : QNetworkInterface::allAddresses()) {
            if (address != QHostAddress(QHostAddress::LocalHost) && address != QHostAddress(QHostAddress::LocalHostIPv6))
                ipAdressList.append(address.toString());
        }

        QHash<QString, QVariant> componentData;
        componentData.insert(ModuleManagerController::s_entityNameComponentName, ModuleManagerController::s_entityName);
        componentData.insert(ModuleManagerController::s_entitiesComponentName, QVariant());
        componentData.insert(ModuleManagerController::s_sessionComponentName, QVariant(m_currentSession));
        componentData.insert(ModuleManagerController::s_sessionsAvailableComponentName, QVariant(m_availableSessions));
        componentData.insert(ModuleManagerController::s_notificationMessagesComponentName, QVariant(m_notificationMessages.toJson()));
        componentData.insert(ModuleManagerController::s_loggedComponentsComponentName, QVariantMap());
        componentData.insert(ModuleManagerController::s_modulesPausedComponentName, QVariant(false));
        componentData.insert(ModuleManagerController::s_serverIpComponentName, ipAdressList);

        VeinComponent::ComponentData *initialData=nullptr;
        for(const QString &compName : componentData.keys())
        {
            initialData = new VeinComponent::ComponentData();
            initialData->setEntityId(s_entityId);
            initialData->setCommand(VeinComponent::ComponentData::Command::CCMD_ADD);
            initialData->setComponentName(compName);
            initialData->setNewValue(componentData.value(compName));
            initialData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
            initialData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
            emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, initialData));
        }

        m_initDone = true;
    }
}

void ModuleManagerController::setModulesPaused(bool t_paused)
{
    if(t_paused != m_modulesPaused)
    {
        m_modulesPaused = t_paused;
        emit sigModulesPausedChanged(m_modulesPaused);
    }
}

void ModuleManagerController::handleNotificationMessage(QJsonObject t_message)
{
    Q_ASSERT(t_message.isEmpty() == false);
    VeinComponent::ComponentData *notificationMessagesData = new VeinComponent::ComponentData();
    VeinEvent::CommandEvent *emDataEvent = nullptr;
    notificationMessagesData->setEntityId(s_entityId);
    notificationMessagesData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
    notificationMessagesData->setComponentName(s_notificationMessagesComponentName);
    notificationMessagesData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    notificationMessagesData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);

    QJsonArray tmpArray = m_notificationMessages.array();
    tmpArray.append(t_message);
    m_notificationMessages.setArray(tmpArray);
    notificationMessagesData->setNewValue(m_notificationMessages.toJson());

    emDataEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, notificationMessagesData);
    emit sigSendEvent(emDataEvent);
}

