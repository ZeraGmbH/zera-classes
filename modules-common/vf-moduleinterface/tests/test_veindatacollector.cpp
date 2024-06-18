#include "test_veindatacollector.h"
#include "vl_sqlitedb.h"
#include <timemachineobject.h>
#include <loggercontentsetconfig.h>
#include <jsonloggercontentloader.h>
#include <jsonloggercontentsessionloader.h>
#include <qtest.h>

QTEST_MAIN(test_veinDataCollector)

void test_veinDataCollector::initTestCase()
{
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(":/contentsets/", std::make_shared<JsonLoggerContentLoader>());
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(":/sessions/", std::make_shared<JsonLoggerContentSessionLoader>());
}

void test_veinDataCollector::init()
{
    m_dataLoggerSystemInitialized = false;

    m_testRunner = std::make_unique<ModuleManagerTestRunner>("", true);
    m_storage = m_testRunner->getVeinStorageSystem();

    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    connect(mmFacade->getLicenseSystem(), &LicenseSystemInterface::sigSerialNumberInitialized,
            this, &test_veinDataCollector::onSerialNoLicensed);

    const VeinLogger::DBFactory sqliteFactory = [](){
        return new VeinLogger::SQLiteDB();
    };
    m_dataLoggerSystem = std::make_unique<VeinLogger::DatabaseLogger>(mmFacade->getStorageSystem(), sqliteFactory);

    startModman("session-minimal-rms.json");

    m_dataCollector = new VeinDataCollector(m_storage);
}

void test_veinDataCollector::cleanup()
{
    m_testRunner = nullptr;
    m_dataLoggerSystem = nullptr;
    delete m_dataCollector;
    TimeMachineObject::feedEventLoop();
}

void test_veinDataCollector::storeRmsValues()
{
    m_dataCollector->collectRMSValues();

    int rmsEntity = 1040;
    emit m_storage->sigSendEvent(generateEvent(rmsEntity, "ACT_RMSPN1", QVariant(), 10) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntity, "ACT_RMSPN2", QVariant(), 11) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntity, "ACT_RMSPN3", QVariant(), 12) );
    TimeMachineObject::feedEventLoop();

    QHash<QString, QList<QVariant>> storedValue = m_dataCollector->getStoredValuesOfEntity(rmsEntity);
    QCOMPARE(storedValue.size(), 3);
    QVERIFY(storedValue.contains("ACT_RMSPN1"));
    QVERIFY(storedValue["ACT_RMSPN1"].contains(10));
    QVERIFY(storedValue.contains("ACT_RMSPN2"));
    QVERIFY(storedValue["ACT_RMSPN2"].contains(11));
    QVERIFY(storedValue.contains("ACT_RMSPN3"));
    QVERIFY(storedValue["ACT_RMSPN3"].contains(12));

    emit m_storage->sigSendEvent(generateEvent(rmsEntity, "ACT_RMSPN1", QVariant(), 20) );
    emit m_storage->sigSendEvent(generateEvent(rmsEntity, "ACT_RMSPN1", QVariant(), 30) );
    TimeMachineObject::feedEventLoop();

    storedValue = m_dataCollector->getStoredValuesOfEntity(rmsEntity);
    QVERIFY(storedValue.contains("ACT_RMSPN1"));
    QVERIFY(storedValue["ACT_RMSPN1"].contains(10));
    QVERIFY(storedValue["ACT_RMSPN1"].contains(20));
    QVERIFY(storedValue["ACT_RMSPN1"].contains(30));
}

void test_veinDataCollector::storePowerValues()
{
    m_dataCollector->collectPowerValuesForEmobAC();

    int power1Entity = 1070;
    emit m_storage->sigSendEvent(generateEvent(power1Entity, "ACT_PQS1", QVariant(), 1) );
    emit m_storage->sigSendEvent(generateEvent(power1Entity, "ACT_PQS2", QVariant(), 2) );
    emit m_storage->sigSendEvent(generateEvent(power1Entity, "ACT_PQS3", QVariant(), 3) );
    emit m_storage->sigSendEvent(generateEvent(power1Entity, "ACT_PQS4", QVariant(), 4) );
    TimeMachineObject::feedEventLoop();

    QHash<QString, QList<QVariant>> storedValue = m_dataCollector->getStoredValuesOfEntity(power1Entity);
    QCOMPARE(storedValue.size(), 4);
    QVERIFY(storedValue.contains("ACT_PQS1"));
    QVERIFY(storedValue["ACT_PQS1"].contains(1));
    QVERIFY(storedValue.contains("ACT_PQS2"));
    QVERIFY(storedValue.contains("ACT_PQS3"));
    QVERIFY(storedValue.contains("ACT_PQS4"));

    emit m_storage->sigSendEvent(generateEvent(power1Entity, "ACT_PQS3", QVariant(), 5.5) );
    emit m_storage->sigSendEvent(generateEvent(power1Entity, "ACT_PQS3", QVariant(), 4) );
    TimeMachineObject::feedEventLoop();

    storedValue = m_dataCollector->getStoredValuesOfEntity(power1Entity);
    QVERIFY(storedValue.contains("ACT_PQS3"));
    QVERIFY(storedValue["ACT_PQS3"].contains(3));
    QVERIFY(storedValue["ACT_PQS3"].contains(5.5));
    QVERIFY(storedValue["ACT_PQS3"].contains(4));

}

void test_veinDataCollector::onSerialNoLicensed()
{
    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    LicenseSystemInterface *licenseSystem = mmFacade->getLicenseSystem();
    if(licenseSystem->isSystemLicensed(m_dataLoggerSystem->entityName())) {
        if(!m_dataLoggerSystemInitialized) {
            m_dataLoggerSystemInitialized = true;
            qInfo("Starting DataLoggerSystem...");
            mmFacade->addSubsystem(m_dataLoggerSystem.get());
        }
    }
}

void test_veinDataCollector::startModman(QString sessionFileName)
{
    m_testRunner->start(":/sessions/" + sessionFileName);
    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    mmFacade->getLicenseSystem()->setDeviceSerial("foo");
    TimeMachineObject::feedEventLoop();
    mmFacade->getSystemModuleEventSystem()->initializeEntity(sessionFileName, QStringList() << sessionFileName);
    TimeMachineObject::feedEventLoop();
}

QEvent *test_veinDataCollector::generateEvent(int entityId, QString componentName, QVariant oldValue, QVariant newValue)
{
    VeinComponent::ComponentData *cData = new VeinComponent::ComponentData(entityId, VeinComponent::ComponentData::Command::CCMD_SET);
    cData->setEventOrigin(VeinComponent::ComponentData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinComponent::ComponentData::EventTarget::ET_ALL);
    cData->setComponentName(componentName);

    cData->setNewValue(newValue);
    cData->setOldValue(oldValue);

    return new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, cData);
}

