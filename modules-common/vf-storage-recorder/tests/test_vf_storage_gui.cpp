#include "test_vf_storage_gui.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_vf_storage_gui)

static constexpr int storageEntityId = 1;
static constexpr int maximumStorage = 5;

void test_vf_storage_gui::init()
{
    m_eventHandler = std::make_unique<VeinEvent::EventHandler>();
    m_storageEventSystem = std::make_shared<VeinStorage::StorageEventSystem>();
    m_storageRecorder = std::make_unique<Vf_Storage>(m_storageEventSystem.get());

    m_eventHandler->addSubsystem(m_storageEventSystem.get());
    m_eventHandler->addSubsystem(m_storageRecorder->getVeinEntity());
    m_storageRecorder->initOnce();
    TimeMachineObject::feedEventLoop();
}

void test_vf_storage_gui::cleanup()
{
    m_eventHandler = nullptr;
    m_storageRecorder = nullptr;
    m_storageEventSystem = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_vf_storage_gui::componentsFound()
{
    QList<QString> storageComponents = m_storageEventSystem->getDb()->getComponentList(storageEntityId);

    QCOMPARE(storageComponents.count(), 17);
    QVERIFY(storageComponents.contains("EntityName"));
    for(int i = 0; i < maximumStorage; i++) {
        QVERIFY(storageComponents.contains(QString("StoredValues%1").arg(i)));
        QVERIFY(storageComponents.contains(QString("PAR_JsonWithEntities%1").arg(i)));
        QVERIFY(storageComponents.contains(QString("PAR_StartStopLogging%1").arg(i)));
    }
}

void test_vf_storage_gui::storeValuesBasedOnNoEntitiesInJson()
{
    for(int i = 0; i < maximumStorage; i++) {
        changeComponentValue(storageEntityId, QString("PAR_JsonWithEntities%1").arg(i), QVariant(), "");
        changeComponentValue(storageEntityId, QString("PAR_StartStopLogging%1").arg(i), QVariant(), "");

        QJsonObject storedValues = m_storageEventSystem->getDb()->getStoredValue(storageEntityId, QString("StoredValues%1").arg(i)).toJsonObject();
        QVERIFY(storedValues.isEmpty());
    }
}

void test_vf_storage_gui::storeValuesBasedOnIncorrectEntitiesInJson()
{
    startLoggingFromJson(":/incorrect-entities.json", 0);
    QJsonObject storedValues = m_storageEventSystem->getDb()->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();

    QVERIFY(storedValues.isEmpty());
}

void test_vf_storage_gui::changeComponentValue(int entityId, QString componentName, QVariant oldValue, QVariant newValue)
{
    emit m_storageEventSystem->sigSendEvent(generateEvent(entityId, componentName, QVariant(), newValue));
    TimeMachineObject::feedEventLoop();
}

QEvent *test_vf_storage_gui::generateEvent(int entityId, QString componentName, QVariant oldValue, QVariant newValue)
{
    VeinComponent::ComponentData *cData = new VeinComponent::ComponentData(entityId, VeinComponent::ComponentData::Command::CCMD_SET);
    cData->setEventOrigin(VeinComponent::ComponentData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinComponent::ComponentData::EventTarget::ET_ALL);
    cData->setComponentName(componentName);

    cData->setNewValue(newValue);
    cData->setOldValue(oldValue);

    return new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, cData);
}

QString test_vf_storage_gui::readEntitiesAndCompoFromJsonFile(QString filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    return file.readAll();
}

void test_vf_storage_gui::startLoggingFromJson(QString fileName, int storageNum)
{
    QString fileContent = readEntitiesAndCompoFromJsonFile(fileName);
    changeComponentValue(storageEntityId, QString("PAR_JsonWithEntities%1").arg(storageNum), QVariant(), fileContent);
    changeComponentValue(storageEntityId, QString("PAR_StartStopLogging%1").arg(storageNum), QVariant(), true);
}

