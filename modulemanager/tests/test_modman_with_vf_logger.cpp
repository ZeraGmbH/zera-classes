#include "test_modman_with_vf_logger.h"
#include "jsonloggercontentloader.h"
#include "jsonloggercontentsessionloader.h"
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

void test_modman_with_vf_logger::checkEntitiesCreated()
{
    VeinEvent::StorageSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    QList<int> entityList = veinStorage->getEntityList();

    QCOMPARE(entityList.count(), 4);
    QVERIFY(entityList.contains(systemEntityId));
    QVERIFY(entityList.contains(scriptEntityId));
    QVERIFY(entityList.contains(dataLoggerEntityId));
    QVERIFY(entityList.contains(rmsEntityId));
}

void test_modman_with_vf_logger::checkLoggerComponentsCreated()
{
    VeinEvent::StorageSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    QList<QString> loggerComponents = veinStorage->getEntityComponents(dataLoggerEntityId);
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

void test_modman_with_vf_logger::onVfQmlStateChanged(VeinApiQml::VeinQml::ConnectionState t_state)
{
    if(t_state == VeinApiQml::VeinQml::ConnectionState::VQ_LOADED && m_initQmlSystemOnce == false) {
        m_scriptSystem->loadScriptFromFile(":/data_logger.qml");
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

void test_modman_with_vf_logger::createModmanWithLogger()
{
    m_initQmlSystemOnce = false;
    m_dataLoggerSystemInitialized = false;

    m_testRunner = std::make_unique<ModuleManagerTestRunner>("", true);
    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    m_scriptSystem = std::make_unique<VeinScript::ScriptSystem>();
    m_qmlSystem = std::make_unique<VeinApiQml::VeinQml>();

    mmFacade->addSubsystem(m_qmlSystem.get());
    mmFacade->addSubsystem(m_scriptSystem.get());

    connect(m_qmlSystem.get(), &VeinApiQml::VeinQml::sigStateChanged,
            this, &test_modman_with_vf_logger::onVfQmlStateChanged);

    LicenseSystemInterface *licenseSystem = mmFacade->getLicenseSystem();
    connect(licenseSystem, &LicenseSystemInterface::sigSerialNumberInitialized,
            this, &test_modman_with_vf_logger::onSerialNoLicensed);

    const VeinLogger::DBFactory sqliteFactory = [](){
        return new VeinLogger::SQLiteDB();
    };
    m_dataLoggerSystem = std::make_unique<ZeraDBLogger>(new VeinLogger::DataSource(mmFacade->getStorageSystem()), sqliteFactory); //takes ownership of DataSource

    VeinApiQml::VeinQml::setStaticInstance(m_qmlSystem.get());
    VeinLogger::QmlLogger::setStaticLogger(m_dataLoggerSystem.get());
    VeinLogger::QmlLogger::setJsonEnvironment(MODMAN_CONTENTSET_PATH, std::make_shared<JsonLoggerContentLoader>());
    VeinLogger::QmlLogger::setJsonEnvironment(MODMAN_SESSION_PATH, std::make_shared<JsonLoggerContentSessionLoader>());
}

void test_modman_with_vf_logger::startModman()
{
    m_testRunner->start(":/session-minimal-rms.json");
    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();
    mmFacade->getLicenseSystem()->setDeviceSerial("foo");
    TimeMachineObject::feedEventLoop();
}

void test_modman_with_vf_logger::init()
{
    createModmanWithLogger();
    startModman();
}

void test_modman_with_vf_logger::cleanup()
{
    m_testRunner = nullptr;
}

