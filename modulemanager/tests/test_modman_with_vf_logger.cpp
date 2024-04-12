#include "test_modman_with_vf_logger.h"
#include <jsonloggercontentloader.h>
#include <jsonloggercontentsessionloader.h>
#include <loggercontentsetconfig.h>
#include "vl_qmllogger.h"
#include "vl_sqlitedb.h"
#include "vl_datasource.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_modman_with_vf_logger)

static int constexpr systemEntityId = 0;
static int constexpr scriptEntityId = 1;
static int constexpr dataLoggerEntityId = 2;
static int constexpr rmsEntityId = 1040;

void test_modman_with_vf_logger::entitiesCreated()
{
    startModman("session-minimal-rms.json");

    QList<int> entityList = m_storage->getEntityList();

    QCOMPARE(entityList.count(), 4);
    QVERIFY(entityList.contains(systemEntityId));
    QVERIFY(entityList.contains(scriptEntityId));
    QVERIFY(entityList.contains(dataLoggerEntityId));
    QVERIFY(entityList.contains(rmsEntityId));
}

void test_modman_with_vf_logger::loggerComponentsCreated()
{
    startModman("session-minimal-rms.json");

    QList<QString> loggerComponents = m_storage->getEntityComponents(dataLoggerEntityId);

    QCOMPARE(loggerComponents.count(), 15);
    QVERIFY(loggerComponents.contains("availableContentSets"));
    QVERIFY(loggerComponents.contains("CustomerData"));
    QVERIFY(loggerComponents.contains("currentContentSets"));
    QVERIFY(loggerComponents.contains("DatabaseFile"));
    QVERIFY(loggerComponents.contains("DatabaseReady"));
    QVERIFY(loggerComponents.contains("EntityName"));
    QVERIFY(loggerComponents.contains("ExistingSessions"));
    QVERIFY(loggerComponents.contains("guiContext"));
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

    QStringList availContentSets = m_storage->getStoredValue(dataLoggerEntityId, "availableContentSets").toStringList();
    QCOMPARE(availContentSets.count(), 2);
    QVERIFY(availContentSets.contains("ZeraAll"));
    QVERIFY(availContentSets.contains("ZeraActualValuesTest"));
}

// Lesson learned / TODO: Move LoggedComponents from system- -> logger-entity
void test_modman_with_vf_logger::contentSetsSelectValid()
{
    startModman("session-minimal-rms.json");

    QCOMPARE(m_storage->getStoredValue(dataLoggerEntityId, "currentContentSets"), QStringList());
    QVariantMap loggedComponents = m_storage->getStoredValue(systemEntityId, "LoggedComponents").toMap();
    QCOMPARE(loggedComponents, QVariantMap());

    m_testRunner->setVfComponent(dataLoggerEntityId, "currentContentSets", "ZeraActualValuesTest");

    loggedComponents = m_storage->getStoredValue(systemEntityId, "LoggedComponents").toMap();
    QString rmsEntityNum = QString("%1").arg(rmsEntityId);

    // TODO: On rework, a string not in a list must not be accepted
    QVariant contentSets = m_storage->getStoredValue(dataLoggerEntityId, "currentContentSets");
    QCOMPARE(contentSets, "ZeraActualValuesTest");

    QVERIFY(loggedComponents.contains(rmsEntityNum));
    QCOMPARE(loggedComponents[rmsEntityNum], QStringList()); // no specific components in contentset ZeraActualValuesTest
}

void test_modman_with_vf_logger::contentSetsSelectInvValid()
{
    startModman("session-minimal-rms.json");

    QCOMPARE(m_storage->getStoredValue(dataLoggerEntityId, "currentContentSets"), QStringList());
    QVariantMap loggedComponents = m_storage->getStoredValue(systemEntityId, "LoggedComponents").toMap();
    QCOMPARE(loggedComponents, QVariantMap());

    m_testRunner->setVfComponent(dataLoggerEntityId, "currentContentSets", "Foo");

    loggedComponents = m_storage->getStoredValue(systemEntityId, "LoggedComponents").toMap();
    QVERIFY(loggedComponents.isEmpty());
}

void test_modman_with_vf_logger::contentSetsSelectValidList()
{
    startModman("session-minimal-rms.json");

    QCOMPARE(m_storage->getStoredValue(dataLoggerEntityId, "currentContentSets"), QStringList());

    m_testRunner->setVfComponent(dataLoggerEntityId, "currentContentSets", QStringList() << "ZeraActualValuesTest");

    QVariantMap loggedComponents = m_storage->getStoredValue(systemEntityId, "LoggedComponents").toMap();
    QString rmsEntityNum = QString("%1").arg(rmsEntityId);

    QVariantList contentSets = m_storage->getStoredValue(dataLoggerEntityId, "currentContentSets").toList();
    QCOMPARE(contentSets.size(), 1);
    QCOMPARE(contentSets[0], "ZeraActualValuesTest");

    QVERIFY(loggedComponents.contains(rmsEntityNum));
    QCOMPARE(loggedComponents[rmsEntityNum], QStringList()); // no specific components in contentset ZeraActualValuesTest
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// code to setup test environment
void test_modman_with_vf_logger::initTestCase()
{
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(":/contentsets/", std::make_shared<JsonLoggerContentLoader>());
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(":/sessions/", std::make_shared<JsonLoggerContentSessionLoader>());
}

void test_modman_with_vf_logger::init()
{
    createModmanWithLogger();
}

void test_modman_with_vf_logger::onVfQmlStateChanged(VeinApiQml::VeinQml::ConnectionState t_state)
{
    if(t_state == VeinApiQml::VeinQml::ConnectionState::VQ_LOADED && m_initQmlSystemOnce == false) {
        VeinLogger::DatabaseLogger::loadScripts(m_scriptSystem.get());
        m_initQmlSystemOnce = true;
    }
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

            // subscribe those entitities our magic logger QML script requires
            m_qmlSystem->entitySubscribeById(systemEntityId);
            m_qmlSystem->entitySubscribeById(dataLoggerEntityId);
        }
    }
}

void test_modman_with_vf_logger::cleanup()
{
    m_testRunner = nullptr;
    m_dataLoggerSystem = nullptr;
    TimeMachineObject::feedEventLoop();
    m_scriptSystem = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_modman_with_vf_logger::createModmanWithLogger()
{
    m_initQmlSystemOnce = false;
    m_dataLoggerSystemInitialized = false;

    m_testRunner = std::make_unique<ModuleManagerTestRunner>("", true);
    m_storage = m_testRunner->getVeinStorageSystem();

    m_scriptSystem = std::make_unique<VeinScript::ScriptSystem>();

    m_qmlSystem = std::make_unique<VeinApiQml::VeinQml>();
    VeinApiQml::VeinQml::setStaticInstance(m_qmlSystem.get());

    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    mmFacade->addSubsystem(m_qmlSystem.get());
    mmFacade->addSubsystem(m_scriptSystem.get());

    connect(m_qmlSystem.get(), &VeinApiQml::VeinQml::sigStateChanged,
            this, &test_modman_with_vf_logger::onVfQmlStateChanged);

    connect(mmFacade->getLicenseSystem(), &LicenseSystemInterface::sigSerialNumberInitialized,
            this, &test_modman_with_vf_logger::onSerialNoLicensed);

    const VeinLogger::DBFactory sqliteFactory = [](){
        return new VeinLogger::SQLiteDB();
    };
    m_dataLoggerSystem = std::make_unique<ZeraDBLogger>(new VeinLogger::DataSource(mmFacade->getStorageSystem()), sqliteFactory); //takes ownership of DataSource
    VeinLogger::QmlLogger::setStaticLogger(m_dataLoggerSystem.get());
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
