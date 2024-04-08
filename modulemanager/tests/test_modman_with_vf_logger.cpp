#include "test_modman_with_vf_logger.h"
#include "modulemanagertestrunner.h"
#include "veinqml.h"
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
    ModuleManagerTestRunner testRunner(":/session-minimal-rms.json", true);

    ModuleManagerSetupFacade* mmFacade = testRunner.getModManFacade();

    // Add all the logger crap as is see modulemanager's main.cpp
    VeinScript::ScriptSystem scriptSystem;
    mmFacade->addModuleSystem(&scriptSystem);

    VeinApiQml::VeinQml qmlSystem;
    mmFacade->addModuleSystem(&qmlSystem);

    const VeinLogger::DBFactory sqliteFactory = [](){
        return new VeinLogger::SQLiteDB();
    };
    ZeraDBLogger dataLoggerSystem(new VeinLogger::DataSource(mmFacade->getStorageSystem()), sqliteFactory); //takes ownership of DataSource


    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getEntityList();

    QCOMPARE(entityList.count(), 2);
}
