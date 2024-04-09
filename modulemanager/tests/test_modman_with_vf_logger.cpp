#include "test_modman_with_vf_logger.h"
#include "jsonloggercontentloader.h"
#include "jsonloggercontentsessionloader.h"
#include "modulemanagertestrunner.h"
#include "veinqml.h"
#include "vl_qmllogger.h"
#include "vl_sqlitedb.h"
#include "vsc_scriptsystem.h"
#include "vl_datasource.h"
#include "zeradblogger.h"
#include <QTest>

QTEST_MAIN(test_modman_with_vf_logger)

static int constexpr rmsEntityId = 1040;

void test_modman_with_vf_logger::basicCheckRmsModule()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-rms.json", true);
    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->hasEntity(rmsEntityId));
}

void test_modman_with_vf_logger::setupSytemWithLogger()
{
    ModuleManagerTestRunner testRunner("", true);
    ModuleManagerSetupFacade* mmFacade = testRunner.getModManFacade();

    // Add all the logger crap as is see modulemanager's main.cpp
    VeinScript::ScriptSystem *scriptSystem = new VeinScript::ScriptSystem(mmFacade);
    VeinApiQml::VeinQml *qmlSystem = new VeinApiQml::VeinQml(mmFacade);

    const VeinLogger::DBFactory sqliteFactory = [](){
        return new VeinLogger::SQLiteDB();
    };
    ZeraDBLogger *dataLoggerSystem = new ZeraDBLogger(new VeinLogger::DataSource(mmFacade->getStorageSystem(), mmFacade), sqliteFactory, mmFacade); //takes ownership of DataSource

    //setup logger
    VeinApiQml::VeinQml::setStaticInstance(qmlSystem);
    VeinLogger::QmlLogger::setStaticLogger(dataLoggerSystem);
    VeinLogger::QmlLogger::setJsonEnvironment(MODMAN_CONTENTSET_PATH, std::make_shared<JsonLoggerContentLoader>());
    VeinLogger::QmlLogger::setJsonEnvironment(MODMAN_SESSION_PATH, std::make_shared<JsonLoggerContentSessionLoader>());

    bool initQmlSystemOnce = false;
    QObject::connect(qmlSystem, &VeinApiQml::VeinQml::sigStateChanged, [&](VeinApiQml::VeinQml::ConnectionState t_state){
        if(t_state == VeinApiQml::VeinQml::ConnectionState::VQ_LOADED && initQmlSystemOnce == false)
        {
            scriptSystem->loadScriptFromFile(":/data_logger.qml");
            initQmlSystemOnce = true;
        }
    });

    mmFacade->addSubsystem(qmlSystem);
    mmFacade->addSubsystem(scriptSystem);

    LicenseSystemInterface *licenseSystem = mmFacade->getLicenseSystem();
    bool dataLoggerSystemInitialized = false;
    QObject::connect(licenseSystem, &LicenseSystemInterface::sigSerialNumberInitialized, [&](){
        if(licenseSystem->isSystemLicensed(dataLoggerSystem->entityName()))
        {
            if(!dataLoggerSystemInitialized)
            {
                dataLoggerSystemInitialized = true;
                qInfo("Starting DataLoggerSystem...");
                mmFacade->addSubsystem(dataLoggerSystem);

                // subscribe those entitities our magic logger QML script
                // requires (see modMan->loadScripts above)
                qmlSystem->entitySubscribeById(0); //0 = mmController
                qmlSystem->entitySubscribeById(2); //2 = dataLoggerSystem
            }
        }
    });
    testRunner.start(":/session-minimal-rms.json");


    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getEntityList();

    QCOMPARE(entityList.count(), 3);
}
