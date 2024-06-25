#include "QJsonDocument"
#include "vf_storage.h"

Vf_Storage::Vf_Storage(VeinEvent::StorageSystem *storageSystem, QObject *parent, int entityId):
    QObject(parent),
    m_isInitalized(false)
{
    m_entity=new VfCpp::VfCppEntity(entityId);
    m_dataCollect = new VeinDataCollector(storageSystem);
    connect(m_dataCollect, &VeinDataCollector::newStoredValue, this, &Vf_Storage::updateValue);
}

bool Vf_Storage::initOnce()
{
    if(!m_isInitalized) {
        m_isInitalized=true;
        m_entity->initModule();
        m_entity->createComponent("EntityName", "Storage", true);
        m_storedValues = m_entity->createComponent("StoredValues", QJsonObject(), true);
        m_JsonWithEntities = m_entity->createComponent("PAR_JsonWithEntities", "", false);
        m_startStopLogging = m_entity->createComponent("PAR_StartStopLogging", false, false);

        connect(m_startStopLogging.get(), &VfCpp::VfCppComponent::sigValueChanged, this, &Vf_Storage::startStopLogging);
    }
    return true;
}

VfCpp::VfCppEntity *Vf_Storage::getVeinEntity() const
{
    return m_entity;
}

void Vf_Storage::setVeinEntity(VfCpp::VfCppEntity *entity)
{
    m_entity = entity;
}

void Vf_Storage::updateValue(QJsonObject value)
{
    m_storedValues->setValue(value);
}

void Vf_Storage::startStopLogging(QVariant value)
{
    bool onOff = value.toBool();

    if(onOff) {
        m_JsonWithEntities->changeComponentReadWriteType(true);
        QString jsonString = m_JsonWithEntities->getValue().toString();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
        QJsonObject jsonObject = jsonDoc.object();
        readJson(jsonObject);
    }
    else {
        m_JsonWithEntities->changeComponentReadWriteType(false);
        m_dataCollect->stopLogging();
    }
}

void Vf_Storage::readJson(QVariant value)
{
    QJsonObject jsonObject = value.toJsonObject();

    if(!jsonObject.isEmpty()) {
        QHash<int, QStringList> entitesAndComponents = extractEntitiesAndComponents(jsonObject);
        m_dataCollect->startLogging(entitesAndComponents);
    }
    else {
        qInfo("Empty Json !");
    }
}

QHash<int, QStringList> Vf_Storage::extractEntitiesAndComponents(QJsonObject jsonObject)
{
    QHash<int, QStringList> entitesAndComponents;
    QString firstKey = jsonObject.keys().at(0);
    QJsonArray values = jsonObject.value(firstKey).toArray();

    for (const QJsonValue& value : values) {
        QJsonObject itemObject = value.toObject();
        int entityId = itemObject["EntityId"].toInt();
        QJsonValue componentValue = itemObject["Component"];

        QStringList componentList;
        if (componentValue.isArray()) {
            QJsonArray componentArray = componentValue.toArray();
            for (const QJsonValue& compValue : componentArray) {
                componentList.append(compValue.toString());
            }
        }
        else if (componentValue.isString()) {
            componentList.append(componentValue.toString());
        }
        entitesAndComponents.insert(entityId, componentList);
    }
    return entitesAndComponents;
}

