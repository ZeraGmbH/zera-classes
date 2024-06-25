#include "test_vfstorage.h"
#include "modulemanagertestrunner.h"
#include "testveinserverwithnet.h"
#include <timemachineobject.h>
#include "vf_client_entity_subscriber.h"
#include <QTest>

QTEST_MAIN(test_vfstorage)

static constexpr int systemEntityId = 0;
static constexpr int storageEntityId = 1;
static constexpr int rmsEntityId = 1040;

void test_vfstorage::init()
{
    createModmanWithStorage();
}

void test_vfstorage::cleanup()
{
    m_testRunner = nullptr;
    m_vfStorageEntity = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_vfstorage::entitiesFoundMinimalSession()
{
    startModman(":/session-minimal.json");
    QList<int> entityList = m_storage->getEntityList();

    QCOMPARE(entityList.count(), 2);
    QVERIFY(m_storage->hasEntity(systemEntityId));
    QVERIFY(m_storage->hasEntity(storageEntityId));
}

void test_vfstorage::componentsFound()
{
    startModman(":/session-minimal.json");
    QList<QString> storageComponents = m_storage->getEntityComponents(storageEntityId);

    QCOMPARE(storageComponents.count(), 4);
    QVERIFY(storageComponents.contains("EntityName"));
    QVERIFY(storageComponents.contains("StoredValues"));
    QVERIFY(storageComponents.contains("PAR_JsonWithEntities"));
    QVERIFY(storageComponents.contains("PAR_StartStopLogging"));
}

void test_vfstorage::storeValuesBasedOnNoEntitiesInJson()
{
    startModman(":/session-minimal.json");
    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities", QJsonObject());
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging", true);

    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues").toJsonObject();
    QVERIFY(storedValues.isEmpty());
}

void test_vfstorage::storeValuesBasedOnIncorrectEntitiesInJson()
{
    startModman(":/session-minimal.json");
    QFile file(":/incorrect-entities.json");
    file.open(QIODevice::ReadOnly);
    QString fileContent = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent.toUtf8());
    QJsonObject jsonObject = jsonDoc.object();

    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities", jsonObject);
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging", true);
    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues").toJsonObject();

    QVERIFY(storedValues.isEmpty());
}

void test_vfstorage::storeValuesBasedOnCorrectEntitiesInJson()
{
    startModman(":/session-minimal-rms.json");
    QCOMPARE(m_storage->getEntityList().count(), 3);

    QFile file(":/correct-entities.json");
    file.open(QIODevice::ReadOnly);
    QString fileContent = file.readAll();
    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities", fileContent);
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging", false);

    //other way of setting a vf-compo we need VeinEvent::CommandEvent::EventSubtype::NOTIFICATION
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), 1) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 2) );
    TimeMachineObject::feedEventLoop();

    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues").toJsonObject();
    QVERIFY(storedValues.isEmpty());

    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging", true);
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), 3) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 4) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 5) );
    TimeMachineObject::feedEventLoop();

    storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues").toJsonObject();
    QVERIFY(storedValues.contains(QString::number(rmsEntityId)));

    QVariant componentStored = storedValues.value(QString::number(rmsEntityId)).toVariant();
    QHash<QString, QVariant> compoNames = componentStored.toHash();
    QVERIFY(compoNames.contains("ACT_RMSPN1"));
    QVERIFY(compoNames.contains("ACT_RMSPN2"));

    QVariant value = compoNames.value("ACT_RMSPN1");
    QVERIFY(value.toList().contains(3));

    value = compoNames.value("ACT_RMSPN2");
    QVERIFY(value.toList().contains(4));
    QVERIFY(value.toList().contains(5));

    //deactivate "PAR_StartStopLogging" and expect same content
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging", false);
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), 7) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 8) );
    TimeMachineObject::feedEventLoop();

    storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues").toJsonObject();
    componentStored = storedValues.value(QString::number(rmsEntityId)).toVariant();
    compoNames = componentStored.toHash();
    value = compoNames.value("ACT_RMSPN1");
    QVERIFY(value.toList().contains(3));
    QVERIFY(!value.toList().contains(7));

    value = compoNames.value("ACT_RMSPN2");
    QVERIFY(value.toList().contains(4));
    QVERIFY(value.toList().contains(5));
    QVERIFY(!value.toList().contains(8));
}

void test_vfstorage::onSerialNoLicensed()
{
    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    LicenseSystemInterface *licenseSystem = mmFacade->getLicenseSystem();
    QString entityName = "Storage";
    if(licenseSystem->isSystemLicensed(entityName)) {
        if(!m_dataLoggerSystemInitialized) {
            m_dataLoggerSystemInitialized = true;
            qInfo("Starting DataLoggerSystem...");
            mmFacade->addSubsystem(m_vfStorageEntity->getVeinEntity());
            m_vfStorageEntity->initOnce();
        }
    }
}

void test_vfstorage::createModmanWithStorage()
{
    m_dataLoggerSystemInitialized = false;

    m_testRunner = std::make_unique<ModuleManagerTestRunner>("", true);
    m_storage = m_testRunner->getVeinStorageSystem();

    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    connect(mmFacade->getLicenseSystem(), &LicenseSystemInterface::sigSerialNumberInitialized,
            this, &test_vfstorage::onSerialNoLicensed);
    m_vfStorageEntity = std::make_unique<Vf_Storage>(m_storage);
}

void test_vfstorage::startModman(QString sessionFileName)
{
    m_testRunner->start(sessionFileName);
    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    mmFacade->getLicenseSystem()->setDeviceSerial("foo");
    TimeMachineObject::feedEventLoop();
    mmFacade->getSystemModuleEventSystem()->initializeEntity(sessionFileName, QStringList() << sessionFileName);
    TimeMachineObject::feedEventLoop();
}

QEvent *test_vfstorage::generateEvent(int entityId, QString componentName, QVariant oldValue, QVariant newValue)
{
    VeinComponent::ComponentData *cData = new VeinComponent::ComponentData(entityId, VeinComponent::ComponentData::Command::CCMD_SET);
    cData->setEventOrigin(VeinComponent::ComponentData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinComponent::ComponentData::EventTarget::ET_ALL);
    cData->setComponentName(componentName);

    cData->setNewValue(newValue);
    cData->setOldValue(oldValue);

    return new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, cData);
}
