#include "test_vfstorage.h"
#include "modulemanagertestrunner.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_vfstorage)

static constexpr int systemEntityId = 0;
static constexpr int storageEntityId = 1;
static constexpr int rmsEntityId = 1040;
static constexpr int powerEntityId = 1070;
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
    startLoggingFromJson(":/incorrect-entities.json", 0);
    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();

    QVERIFY(storedValues.isEmpty());
}

void test_vfstorage::storeValuesEmptyComponentsInJson()
{
    constexpr int storageNum = 0;
    startModman(":/session-minimal-rms.json");
    startLoggingFromJson(":/empty-components.json", 0);
    changeRMSValues(1, 2);
    m_testRunner->setVfComponent(rmsEntityId, "PAR_Interval", 5);
    TimeMachineForTest::getInstance()->processTimers(100);

    QJsonObject storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(storageNum);
    QVERIFY(storedValuesWithoutTimeStamp.contains(QString::number(rmsEntityId)));

    QHash<QString, QVariant> componentsHash = getComponentsStoredOfEntity(rmsEntityId, storedValuesWithoutTimeStamp);
    QString value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN1");
    QCOMPARE(value, "1");
    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN2");
    QCOMPARE(value, "2");
    value = getValuesStoredOfComponent(componentsHash, "PAR_Interval");
    QCOMPARE(value, "5");
}

void test_vfstorage::storeValuesCorrectEntitiesStartStopLoggingDisabled()
{
    startModman(":/session-minimal-rms.json");
    QCOMPARE(m_storage->getEntityList().count(), 3);

    QString fileContent = readEntitiesAndCompoFromJsonFile(":/correct-entities.json");
    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities0", fileContent);
    stopLoggingFromJson(0);

    changeRMSValues(1, 2);

    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    QVERIFY(storedValues.isEmpty());
}

void test_vfstorage::loggingOnOffSequence0()
{
    constexpr int storageNum = 0;
    startModman(":/session-minimal-rms.json");
    startLoggingFromJson(":/correct-entities.json", storageNum);

    changeRMSValues(3, 4);
    TimeMachineForTest::getInstance()->processTimers(100);

    QJsonObject storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(storageNum);
    QVERIFY(storedValuesWithoutTimeStamp.contains(QString::number(rmsEntityId)));

    QHash<QString, QVariant> componentsHash = getComponentsStoredOfEntity(rmsEntityId, storedValuesWithoutTimeStamp);
    QString value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN1");
    QCOMPARE(value, "3");
    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN2");
    QCOMPARE(value, "4");

    stopLoggingFromJson(storageNum);
    changeRMSValues(7, 8);
    TimeMachineForTest::getInstance()->processTimers(100);

    storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(storageNum);
    componentsHash = getComponentsStoredOfEntity(rmsEntityId, storedValuesWithoutTimeStamp);

    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN1");
    QCOMPARE(value, "3");  // !=7
    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN2");
    QCOMPARE(value, "4");  // !=8
}


void test_vfstorage::loggingOnOffSequence1()
{
    constexpr int storageNum = 1;
    startModman(":/session-minimal-rms.json");
    startLoggingFromJson(":/correct-entities.json", storageNum);

    changeRMSValues(3, 4);
    TimeMachineForTest::getInstance()->processTimers(100);

    QJsonObject storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(storageNum);
    QVERIFY(storedValuesWithoutTimeStamp.contains(QString::number(rmsEntityId)));

    QHash<QString, QVariant> componentsHash = getComponentsStoredOfEntity(rmsEntityId, storedValuesWithoutTimeStamp);
    QString value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN1");
    QCOMPARE(value, "3");
    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN2");
    QCOMPARE(value, "4");

    stopLoggingFromJson(storageNum);
    changeRMSValues(7, 8);
    TimeMachineForTest::getInstance()->processTimers(100);

    storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(storageNum);
    componentsHash = getComponentsStoredOfEntity(rmsEntityId, storedValuesWithoutTimeStamp);

    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN1");
    QCOMPARE(value, "3");  // !=7
    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN2");
    QCOMPARE(value, "4");  // !=8
}

void test_vfstorage::stopLoggingHasNoSideEffectOnOtherConnections()
{
    constexpr int storageNum = 0;
    startModman(":/session-minimal-rms.json");
    startLoggingFromJson(":/correct-entities.json", storageNum);

    int changesDetected = 0;
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageSystem()->getDb();
    VeinStorage::AbstractComponentPtr component = storageDb->findComponent(rmsEntityId, "ACT_RMSPN1");
    connect(component.get(), &VeinStorage::AbstractComponent::sigValueChange, [&]() {
        changesDetected++;
    });
    changeRMSValues(39, 42);
    QCOMPARE(changesDetected, 1);

    stopLoggingFromJson(storageNum);

    changeRMSValues(42, 39);
    QCOMPARE(changesDetected, 2);
}

void test_vfstorage::changeJsonFileWhileLogging()
{
    startModman(":/session-minimal-rms.json");
    startLoggingFromJson(":/correct-entities.json", 0);

    changeRMSValues(10, 11);
    TimeMachineForTest::getInstance()->processTimers(100);

    QJsonObject storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(0);
    QHash<QString, QVariant> componentsHash = getComponentsStoredOfEntity(rmsEntityId, storedValuesWithoutTimeStamp);
    QString value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN1");
    QCOMPARE(value, "10");

    value = getValuesStoredOfComponent(componentsHash, "ACT_RMSPN2");
    QCOMPARE(value, "11");

    QString fileContent = readEntitiesAndCompoFromJsonFile(":/more-rms-components.json");
    m_testRunner->setVfComponent(storageEntityId, "PAR_JsonWithEntities0", fileContent);
    changeRMSValues(5, 6);
    TimeMachineForTest::getInstance()->processTimers(100);

    storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(0);
    QString inputJsonFile = m_storage->getStoredValue(storageEntityId, "PAR_JsonWithEntities0").toString();
    QVERIFY(!inputJsonFile.contains("ACT_RMSPN3"));
    QVERIFY(!inputJsonFile.contains("ACT_RMSPN4"));
}

void test_vfstorage::fireActualValuesAfterDelayWhileLogging()
{
    startModman(":/session-minimal-rms.json");
    startLoggingFromJson(":/correct-entities.json", 0);
    changeRMSValues(3, 4);
    TimeMachineForTest::getInstance()->processTimers(100);

    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    QStringList timestampKeys = storedValues.keys();
    QCOMPARE (timestampKeys.size(), 1);

    TimeMachineForTest::getInstance()->processTimers(5000);
    changeRMSValues(5, 6);
    TimeMachineForTest::getInstance()->processTimers(100);

    storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    timestampKeys = storedValues.keys();
    QCOMPARE (timestampKeys.size(), 2);

    QDateTime firstTimeStamp = QDateTime::fromString(timestampKeys.first() , "dd-MM-yyyy hh:mm:ss.zzz");
    QDateTime lastTimeStamp = QDateTime::fromString(timestampKeys.last() , "dd-MM-yyyy hh:mm:ss.zzz");
    QVERIFY(firstTimeStamp < lastTimeStamp);
}

void test_vfstorage::fireRmsPowerValuesAfterDifferentDelaysWhileLogging()
{
    startModman(":/session-minimal-rms-power.json");
    startLoggingFromJson(":/rms-power1-components.json", 0);

    changeRMSValues(1, 2);
    emit m_storage->sigSendEvent(generateEvent(1070, "ACT_PQS1", QVariant(), 1) );
    emit m_storage->sigSendEvent(generateEvent(1070, "ACT_PQS2", QVariant(), 2) );
    TimeMachineObject::feedEventLoop();
    TimeMachineForTest::getInstance()->processTimers(100);

    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    QStringList timestampKeys = storedValues.keys();
    QCOMPARE (timestampKeys.size(), 1);

    QJsonObject storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(0);
    QVERIFY(storedValuesWithoutTimeStamp.contains(QString::number(rmsEntityId)));
    QVERIFY(storedValuesWithoutTimeStamp.contains(QString::number(powerEntityId)));

    TimeMachineForTest::getInstance()->processTimers(500);

    changeRMSValues(3, 4);
    TimeMachineForTest::getInstance()->processTimers(100);

    storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    timestampKeys = storedValues.keys();
    QCOMPARE (timestampKeys.size(), 2);

    storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(0);
    QVERIFY(storedValuesWithoutTimeStamp.contains(QString::number(rmsEntityId)));
    QVERIFY(!storedValuesWithoutTimeStamp.contains(QString::number(powerEntityId)));

    TimeMachineForTest::getInstance()->processTimers(500);
    changeRMSValues(5, 6);
    emit m_storage->sigSendEvent(generateEvent(1070, "ACT_PQS1", QVariant(), 5) );
    TimeMachineObject::feedEventLoop();
    TimeMachineForTest::getInstance()->processTimers(10);
    emit m_storage->sigSendEvent(generateEvent(1070, "ACT_PQS2", QVariant(), 6) );

    TimeMachineForTest::getInstance()->processTimers(100);

    storedValues = m_storage->getStoredValue(storageEntityId, "StoredValues0").toJsonObject();
    timestampKeys = storedValues.keys();
    QCOMPARE (timestampKeys.size(), 3);

    storedValuesWithoutTimeStamp = getStoredValueWithoutTimeStamp(0);
    QVERIFY(storedValuesWithoutTimeStamp.contains(QString::number(rmsEntityId)));
    QVERIFY(storedValuesWithoutTimeStamp.contains(QString::number(powerEntityId)));
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

QJsonObject test_vfstorage::getStoredValueWithoutTimeStamp(int storageNum)
{
    QJsonObject storedValuesWithoutTimeStamp;
    QJsonObject storedValues = m_storage->getStoredValue(storageEntityId, QString("StoredValues%1").arg(storageNum)).toJsonObject();
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

void test_vfstorage::startLoggingFromJson(QString fileName, int storageNum)
{
    QString fileContent = readEntitiesAndCompoFromJsonFile(fileName);
    m_testRunner->setVfComponent(storageEntityId, QString("PAR_JsonWithEntities%1").arg(storageNum), fileContent);
    m_testRunner->setVfComponent(storageEntityId, QString("PAR_StartStopLogging%1").arg(storageNum), true);
}

void test_vfstorage::stopLoggingFromJson(int storageNum)
{
    m_testRunner->setVfComponent(storageEntityId, QString("PAR_StartStopLogging%1").arg(storageNum), false);
}

void test_vfstorage::changeRMSValues(QVariant newValue1, QVariant newValue2)
{
    //other way of setting a vf-compo we need VeinEvent::CommandEvent::EventSubtype::NOTIFICATION
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN1", QVariant(), newValue1) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntityId, "ACT_RMSPN2", QVariant(), newValue2) );
    TimeMachineObject::feedEventLoop();

}
