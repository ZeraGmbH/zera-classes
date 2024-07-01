#include "test_vfstorage.h"
#include "modulemanagertestrunner.h"
#include "testveinserverwithnet.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include "vf_client_entity_subscriber.h"
#include <QTest>

QTEST_MAIN(test_vfstorage)

static constexpr int systemEntityId = 0;
static constexpr int storageEntityId = 1;
static constexpr int rmsEntityId = 1040;
static constexpr int maximumStorage = 5;

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

    QCOMPARE(storageComponents.count(), 17);
    QVERIFY(storageComponents.contains("EntityName"));
    for(int i = 0; i < maximumStorage; i++) {
        QVERIFY(storageComponents.contains(QString("StoredValues%1").arg(i)));
        QVERIFY(storageComponents.contains(QString("PAR_JsonWithEntities%1").arg(i)));
        QVERIFY(storageComponents.contains(QString("PAR_StartStopLogging%1").arg(i)));
    }
}

void test_vfstorage::storeValuesBasedOnNoEntitiesInJson()
{
    startModman(":/session-minimal.json");
    for(int i = 0; i < maximumStorage; i++) {
        m_testRunner->setVfComponent(storageEntityId, QString("PAR_JsonWithEntities%1").arg(i), "");
        m_testRunner->setVfComponent(storageEntityId, QString("PAR_StartStopLogging%1").arg(i), true);

        QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, QString("StoredValues%1").arg(i)).toJsonObject();
        QVERIFY(storedValues.isEmpty());
    }
}

void test_vfstorage::storeValuesBasedOnIncorrectEntitiesInJson()
{
    startModman(":/session-minimal.json");
    QString fileContent = readEntitiesAndCompoFromJsonFile(":/incorrect-entities.json");

    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities0", fileContent);
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging0", true);
    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();

    QVERIFY(storedValues.isEmpty());
}

void test_vfstorage::storeValuesCorrectEntitiesStartStopLoggingDisabled()
{
    startModman(":/session-minimal-rms.json");
    QCOMPARE(m_storage->getEntityList().count(), 3);

    QString fileContent = readEntitiesAndCompoFromJsonFile(":/correct-entities.json");
    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities0", fileContent);
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging0", false);

    //other way of setting a vf-compo we need VeinEvent::CommandEvent::EventSubtype::NOTIFICATION
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), 1) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 2) );
    TimeMachineObject::feedEventLoop();

    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    QVERIFY(storedValues.isEmpty());
}

void test_vfstorage::storeValuesStartStopLoggingEnabledDisabled()
{
    startModman(":/session-minimal-rms.json");
    QString fileContent = readEntitiesAndCompoFromJsonFile(":/correct-entities.json");

    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities0", fileContent);
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging0", true);
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), 3) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 4) );
    TimeMachineObject::feedEventLoop();

    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    QJsonObject storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(storedValues);
    QVERIFY(storedValuesWithoutTimeStamp.contains(QString::number(rmsEntityId)));

    QHash<QString, QVariant> componentsHash = getComponentsStoredOfEntity(rmsEntityId, storedValuesWithoutTimeStamp);
    QVERIFY(componentsHash.contains("ACT_RMSPN1"));
    QVERIFY(componentsHash.contains("ACT_RMSPN2"));

    QString value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN1");
    QCOMPARE(value, "3");

    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN2");
    QCOMPARE(value, "4");

    //deactivate "PAR_StartStopLogging" and expect same content
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging0", false);
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), 7) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 8) );
    TimeMachineObject::feedEventLoop();

    storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(storedValues);
    componentsHash = getComponentsStoredOfEntity(rmsEntityId, storedValuesWithoutTimeStamp);

    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN1");
    QCOMPARE(value, "3");
    QVERIFY(value != "7");

    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN2");
    QCOMPARE(value, "4");
    QVERIFY(value != "8");
}

void test_vfstorage::changeJsonFileWhileLogging()
{
    startModman(":/session-minimal-rms.json");
    QString fileContent = readEntitiesAndCompoFromJsonFile(":/correct-entities.json");

    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities0", fileContent);
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging0", true);
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), 10) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 11) );
    TimeMachineObject::feedEventLoop();

    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    QJsonObject storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(storedValues);

    QHash<QString, QVariant> componentsHash = getComponentsStoredOfEntity(rmsEntityId, storedValuesWithoutTimeStamp);
    QVERIFY(componentsHash.contains("ACT_RMSPN1"));
    QVERIFY(componentsHash.contains("ACT_RMSPN2"));

    QString value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN1");
    QCOMPARE(value, "10");

    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN2");
    QCOMPARE(value, "11");

    fileContent = readEntitiesAndCompoFromJsonFile(":/more-rms-components.json");
    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities0", fileContent);
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN3", QVariant(), 5) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN4", QVariant(), 6) );
    TimeMachineObject::feedEventLoop();

    storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(storedValues);

    QString inputJsonFile = m_storage->getStoredValue(storageEntityId, "PAR_JsonWithEntities0").toString();
    QVERIFY(!inputJsonFile.contains("ACT_RMSPN3"));
    QVERIFY(!inputJsonFile.contains("ACT_RMSPN4"));
}

void test_vfstorage::fireActualValuesAfterDelayWhileLogging()
{
    startModman(":/session-minimal-rms.json");
    QString fileContent = readEntitiesAndCompoFromJsonFile(":/correct-entities.json");

    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities0", fileContent);
    m_testRunner->setVfComponent(storageEntityId, "PAR_StartStopLogging0", true);
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), 3) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 4) );
    TimeMachineObject::feedEventLoop();

    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    QStringList timestampKeys = storedValues.keys();
    QCOMPARE (timestampKeys.size(), 1);

    TimeMachineForTest::getInstance()->processTimers(5000);
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), 5) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), 6) );
    TimeMachineObject::feedEventLoop();
    storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    timestampKeys = storedValues.keys();
    QCOMPARE (timestampKeys.size(), 2);

    QDateTime firstTimeStamp = QDateTime::fromString(timestampKeys.first() , "dd-MM-yyyy hh:mm:ss");
    QDateTime lastTimeStamp = QDateTime::fromString(timestampKeys.last() , "dd-MM-yyyy hh:mm:ss");
    QVERIFY(firstTimeStamp < lastTimeStamp);
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

QString test_vfstorage::readEntitiesAndCompoFromJsonFile(QString filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    return file.readAll();
}

QJsonObject test_vfstorage::getStoredValueWithoutTimeStamp(QJsonObject storedValues)
{
    QJsonObject storedValuesWithoutTimeStamp;
    for(const QString &key : storedValues.keys()) {
        QJsonValue entityFound = storedValues.value(key);
        storedValuesWithoutTimeStamp = entityFound.toObject();
    }
    return storedValuesWithoutTimeStamp;
}

QHash<QString, QVariant> test_vfstorage::getComponentsStoredOfEntity(int entityId, QJsonObject storedValueWithoutTimeStamp)
{
    QVariant componentStored = storedValueWithoutTimeStamp.value(QString::number(entityId)).toVariant();
    return componentStored.toHash();
}

QString test_vfstorage::getValuesStoredOfComponent(QHash<QString, QVariant> componentHash, QString componentName)
{
    QVariant value = componentHash.value(componentName);
    return value.toString();
}
