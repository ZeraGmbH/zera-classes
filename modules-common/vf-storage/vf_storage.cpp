#include "QJsonDocument"
#include "vf_storage.h"

static constexpr int maximumStorages = 5;

Vf_Storage::Vf_Storage(VeinEvent::StorageSystem *storageSystem, QObject *parent, int entityId):
    QObject(parent),
    m_storageSystem(storageSystem),
    m_isInitalized(false)
{
    m_entity=new VfCpp::VfCppEntity(entityId);
    for(int i = 0; i < maximumStorages; i++) {
        VeinDataCollector* dataCollector = new VeinDataCollector(storageSystem);
        m_dataCollect.append(dataCollector);
        connect(dataCollector, &VeinDataCollector::newStoredValue, this, [=](QJsonObject value){
            m_storedValues[i]->setValue(value);
        });
    }
}

bool Vf_Storage::initOnce()
{
    if(!m_isInitalized) {
        m_isInitalized=true;
        m_entity->initModule();
        m_entity->createComponent("EntityName", "Storage", true);
        m_maximumLoggingComponents = m_entity->createComponent("ACT_MaximumLoggingComponents", maximumStorages, true);
        for(int i = 0; i < maximumStorages; i++) {
            m_storedValues.append(m_entity->createComponent(QString("StoredValues%1").arg(i), QJsonObject(), true));
            m_JsonWithEntities.append(m_entity->createComponent(QString("PAR_JsonWithEntities%1").arg(i), "", false));
            m_startStopLogging.append(m_entity->createComponent(QString("PAR_StartStopLogging%1").arg(i), false, false));
            connect(m_startStopLogging.at(i).get(), &VfCpp::VfCppComponent::sigValueChanged, this, [=](QVariant value){
                startStopLogging(value, i);
            });
        }
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

void Vf_Storage::startStopLogging(QVariant value, int storageNum)
{
    bool onOff = value.toBool();

    if(onOff) {
        m_JsonWithEntities[storageNum]->changeComponentReadWriteType(true);
        QString jsonString = m_JsonWithEntities[storageNum]->getValue().toString();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
        QJsonObject jsonObject = jsonDoc.object();
        readJson(jsonObject, storageNum);
    }
    else {
        m_JsonWithEntities[storageNum]->changeComponentReadWriteType(false);
        m_dataCollect[storageNum]->stopLogging();
    }
}

void Vf_Storage::readJson(QVariant value, int storageNum)
{
    QJsonObject jsonObject = value.toJsonObject();

    if(!jsonObject.isEmpty()) {
        QHash<int, QStringList> entitesAndComponents = extractEntitiesAndComponents(jsonObject);
        m_dataCollect[storageNum]->startLogging(entitesAndComponents);
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
            if(componentArray.isEmpty()) {
                componentList = m_storageSystem->getEntityComponents(entityId);
                ignoreComponents(&componentList);
            }
            else {
                for (const QJsonValue& compValue : componentArray) {
                    componentList.append(compValue.toString());
                }
            }
        }
        else if (componentValue.isString()) {
            componentList.append(componentValue.toString());
        }
        entitesAndComponents.insert(entityId, componentList);
    }
    return entitesAndComponents;
}

void Vf_Storage::ignoreComponents(QStringList *componentList)
{
    QString componentToBeIgnored = "SIG_Measuring";
    componentList->removeAll(componentToBeIgnored);
}

