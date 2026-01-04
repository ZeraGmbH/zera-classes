#include "test_modman_with_vf_logger.h"
#include <contentsetsotherfromcontentsetsconfig.h>
#include <contentsetszeraallfrommodmansessions.h>
#include <loggercontentsetconfig.h>
#include "vl_sqlitedb.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_modman_with_vf_logger)

static int constexpr systemEntityId = 0;
static int constexpr dataLoggerEntityId = 2;
static int constexpr rmsEntityId = 1040;

void test_modman_with_vf_logger::entitiesCreated()
{
    startModman("session-minimal-rms.json");

    QList<int> entityList = m_storage->getDb()->getEntityList();

    QCOMPARE(entityList.count(), 3);
    QVERIFY(entityList.contains(systemEntityId));
    QVERIFY(entityList.contains(dataLoggerEntityId));
    QVERIFY(entityList.contains(rmsEntityId));
}

void test_modman_with_vf_logger::loggerComponentsCreated()
{
    startModman("session-minimal-rms.json");

    QList<QString> loggerComponents = m_storage->getDb()->getComponentList(dataLoggerEntityId);

    QCOMPARE(loggerComponents.count(), 15);
    QVERIFY(loggerComponents.contains("availableContentSets"));
    QVERIFY(loggerComponents.contains("currentContentSets"));
    QVERIFY(loggerComponents.contains("DatabaseFile"));
    QVERIFY(loggerComponents.contains("DatabaseReady"));
    QVERIFY(loggerComponents.contains("EntityName"));
    QVERIFY(loggerComponents.contains("ExistingSessions"));
    QVERIFY(loggerComponents.contains("guiContext"));
    QVERIFY(loggerComponents.contains("LoggedComponents"));
    QVERIFY(loggerComponents.contains("LoggingEnabled"));
    QVERIFY(loggerComponents.contains("LoggingStatus"));
    QVERIFY(loggerComponents.contains("ScheduledLoggingCountdown"));
    QVERIFY(loggerComponents.contains("ScheduledLoggingDuration"));
    QVERIFY(loggerComponents.contains("ScheduledLoggingEnabled"));
    QVERIFY(loggerComponents.contains("sessionName"));
    QVERIFY(loggerComponents.contains("transactionName"));
}

void test_modman_with_vf_logger::contentSetsAvailable()
{
    startModman("session-minimal-rms.json");

    QStringList availContentSets = m_storage->getDb()->getStoredValue(dataLoggerEntityId, "availableContentSets").toStringList();
    QCOMPARE(availContentSets.count(), 2);
    QVERIFY(availContentSets.contains("ZeraAll"));
    QVERIFY(availContentSets.contains("ZeraActualValuesTest"));
}

void test_modman_with_vf_logger::contentSetsSelectValid()
{
    startModman("session-minimal-rms.json");

    QCOMPARE(m_storage->getDb()->getStoredValue(dataLoggerEntityId, "currentContentSets"), QVariantList());

    m_testRunner->setVfComponent(dataLoggerEntityId, "currentContentSets", "ZeraActualValuesTest");

    VeinStorage::AbstractDatabase* storageDb = m_storage->getDb();
    QVariantMap loggedComponents = storageDb->getStoredValue(dataLoggerEntityId, "LoggedComponents").toMap();
    QString rmsEntityNum = QString("%1").arg(rmsEntityId);

    // TODO: On rework, a string not in a list must not be accepted
    QVariant contentSets = storageDb->getStoredValue(dataLoggerEntityId, "currentContentSets");
    QCOMPARE(contentSets, "ZeraActualValuesTest");

    QVERIFY(loggedComponents.contains(rmsEntityNum));
    QCOMPARE(loggedComponents[rmsEntityNum], QStringList()); // no specific components in contentset ZeraActualValuesTest
}

void test_modman_with_vf_logger::contentSetsSelectInvalid()
{
    startModman("session-minimal-rms.json");

    QCOMPARE(m_storage->getDb()->getStoredValue(dataLoggerEntityId, "currentContentSets"), QVariantList());

    m_testRunner->setVfComponent(dataLoggerEntityId, "currentContentSets", "Foo");

    VeinStorage::AbstractDatabase* storageDb = m_storage->getDb();
    QVariantMap loggedComponents = storageDb->getStoredValue(dataLoggerEntityId, "LoggedComponents").toMap();
    QVERIFY(loggedComponents.isEmpty());
}

void test_modman_with_vf_logger::contentSetsSelectValidList()
{
    startModman("session-minimal-rms.json");

    QCOMPARE(m_storage->getDb()->getStoredValue(dataLoggerEntityId, "currentContentSets"), QVariantList());

    m_testRunner->setVfComponent(dataLoggerEntityId, "currentContentSets", QStringList() << "ZeraActualValuesTest");

    VeinStorage::AbstractDatabase* storageDb = m_storage->getDb();
    QVariantMap loggedComponents = storageDb->getStoredValue(dataLoggerEntityId, "LoggedComponents").toMap();
    QString rmsEntityNum = QString("%1").arg(rmsEntityId);

    QVariantList contentSets = storageDb->getStoredValue(dataLoggerEntityId, "currentContentSets").toList();
    QCOMPARE(contentSets.size(), 1);
    QCOMPARE(contentSets[0], "ZeraActualValuesTest");

    QVERIFY(loggedComponents.contains(rmsEntityNum));
    QCOMPARE(loggedComponents[rmsEntityNum], QStringList()); // no specific components in contentset ZeraActualValuesTest
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// code to setup test environment
void test_modman_with_vf_logger::initTestCase()
{
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(":/contentsets/", std::make_shared<ContentSetsOtherFromContentSetsConfig>());
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(":/sessions/", std::make_shared<ContentSetsZeraAllFromModmanSessions>());
}

void test_modman_with_vf_logger::init()
{
    createModmanWithLogger();
}

void test_modman_with_vf_logger::onSerialNoLicensed()
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

void test_modman_with_vf_logger::cleanup()
{
    m_testRunner = nullptr;
    m_dataLoggerSystem = nullptr;
    TimeMachineObject::feedEventLoop();
    TimeMachineObject::feedEventLoop();
}

void test_modman_with_vf_logger::createModmanWithLogger()
{
    m_dataLoggerSystemInitialized = false;

    m_testRunner = std::make_unique<ModuleManagerTestRunner>("", true);
    m_storage = m_testRunner->getVeinStorageSystem();

    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    connect(mmFacade->getLicenseSystem(), &LicenseSystemInterface::sigSerialNumberInitialized,
            this, &test_modman_with_vf_logger::onSerialNoLicensed);

    const VeinLogger::DBFactory sqliteFactory = [](){
        return std::make_shared<VeinLogger::SQLiteDB>();
    };
    m_dataLoggerSystem = std::make_unique<VeinLogger::DatabaseLogger>(mmFacade->getStorageSystem(), sqliteFactory);
}

void test_modman_with_vf_logger::startModman(QString sessionFileName)
{
    m_testRunner->start(":/sessions/" + sessionFileName);
    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    mmFacade->getLicenseSystem()->setDeviceSerial("foo");
    TimeMachineObject::feedEventLoop();
    mmFacade->getSystemModuleEventSystem()->initializeEntity(sessionFileName, QStringList() << sessionFileName);
    TimeMachineObject::feedEventLoop();
}
