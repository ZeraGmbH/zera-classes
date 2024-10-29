#include "test_modman_regression_all_sessions.h"
#include "testlicensesystem.h"
#include "modulemanager.h"
#include "moduledata.h"
#include "modulemanagerconfig.h"
#include "testfactoryserviceinterfaces.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_modman_regression_all_sessions)

void test_modman_regression_all_sessions::initTestCase()
{
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    TestModuleManager::enableTests();
    TestModuleManager::pointToInstalledSessionFiles();
    qputenv("QT_FATAL_CRITICALS", "1");
    qputenv("QT_LOGGING_RULES", "*.debug=false;*.info=false");
}

void test_modman_regression_all_sessions::checkScpiPortOpenedProperlyByVeinDevIface(ModuleManagerSetupFacade &modManFacade)
{
    QString actDevIface = modManFacade.getStorageSystem()->getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");
}

void test_modman_regression_all_sessions::checkVeinModuleEntityNames(TestModuleManager &modMan, ModuleManagerSetupFacade &modManFacade, QString device, QString session)
{
    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    bool allEntitiesOk = true;
    for(int i=0; i<modules.count(); i++) {
        ZeraModules::ModuleData * module = modules[i];
        QString moduleNameExpected = module->m_module->getVeinModuleName();
        QString veinModuleNameFound = modManFacade.getStorageSystem()->getStoredValue(module->m_moduleId, "EntityName").toString();
        if(moduleNameExpected != veinModuleNameFound) {
            allEntitiesOk = false;
            qWarning("Vein entity name incorrect: Device: '%s' / Session '%s' / Expected: '%s' / Received: '%s'",
                     qPrintable(device),
                     qPrintable(session),
                     qPrintable(moduleNameExpected),
                     qPrintable(veinModuleNameFound));
        }
    }
    QVERIFY(allEntitiesOk);
}

int test_modman_regression_all_sessions::generateCodeLists(QString device)
{
    ModulemanagerConfig::setDemoDevice(device, true);
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    QStringList sessions = mmConfig->getAvailableSessions();
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);
    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices(device, false);
    for(const auto &session : sessions) {
        modMan.changeSessionFile(session);
        modMan.waitUntilModulesAreReady();

        checkScpiPortOpenedProperlyByVeinDevIface(modManSetupFacade);
        checkVeinModuleEntityNames(modMan, modManSetupFacade, device, session);

        QStringList codeLines = generateCodeLinesForDeviceSession(device, session, modMan);
        qWarning("%s", qPrintable(codeLines.join("\n")));

        modMan.destroyModulesAndWaitUntilAllShutdown();
    }
    return sessions.count();
}

QStringList test_modman_regression_all_sessions::generateCodeLinesForDeviceSession(QString device, QString session, TestModuleManager &modMan)
{
    QStringList codeLines;
    codeLines.append(QString("// ------ Start auto generated code for %1 ----").arg(session));
    codeLines.append("TestLicenseSystem licenseSystem;");
    codeLines.append("ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);");
    codeLines.append("");
    codeLines.append("TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);");
    codeLines.append("modMan.loadAllAvailableModulePlugins();");
    codeLines.append("modMan.setupConnections();");
    codeLines.append("modMan.startAllTestServices(\"" + device + "\", false);");
    codeLines.append("modMan.changeSessionFile(\"" + session + "\");");
    codeLines.append("modMan.waitUntilModulesAreReady();");
    codeLines.append("");
    codeLines.append("QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();");

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    codeLines.append(QString("QCOMPARE(modules.count(), %1);").arg(modules.count()));
    for(int i=0; i<modules.count(); i++) {
        codeLines.append("");
        codeLines.append(QString("QCOMPARE(modules[%1]->m_moduleId, %2);").arg(i).arg(modules[i]->m_moduleId));
        codeLines.append(QString("QCOMPARE(modules[%1]->m_module->getVeinModuleName(), \"%2\");").arg(i).arg(qPrintable(modules[i]->m_module->getVeinModuleName())));
        codeLines.append(QString("QCOMPARE(modules[%1]->m_module->getSCPIModuleName(), \"%2\");").arg(i).arg(qPrintable(modules[i]->m_module->getSCPIModuleName())));
    }
    codeLines.append("");
    codeLines.append("modMan.destroyModulesAndWaitUntilAllShutdown();");
    codeLines.append("// ------ end auto generated code ----");
    codeLines.append("");
    return codeLines;
}

void test_modman_regression_all_sessions::loadAllSessionsAndOutputRegressionTestCodeCom5003()
{
    int sessionCount = generateCodeLists("com5003");
    constexpr int sessionCountCom5003 = 4;
    if(sessionCount != sessionCountCom5003)
        qFatal("Seems sessions (found %i) changed for Com5003. Read comment below howto update regression tests", sessionCount);
}

void test_modman_regression_all_sessions::loadAllSessionsAndOutputRegressionTestCodeMt310s2()
{
    int sessionCount = generateCodeLists("mt310s2");
    constexpr int sessionCountMt310s2 = 5;
    if(sessionCount != sessionCountMt310s2)
        qFatal("Seems sessions (found %i) changed for MT310s2. Read comment below howto update regression tests", sessionCount);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Contents of functions below are auto-generated
// To update them:
// 1. If number of sessions for a device changed: Adjust numbers in
//    loadAllSessionsAndOutputRegressionTestCode<device> above
// 2. If a session was added for a device: Add a new (empty) regression test
// 3. Run test_modman_regression_all_sessions in qt-creator - should pass with 1-2.
// 4. Copy matching (warning) log from loadAllSessionsAndOutputRegressionTestCode<device>
//    in session/device matching test below

void test_modman_regression_all_sessions::regressionCom5003Ced()
{
    // ------ Start auto generated code for com5003-ced-session.json ----
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("com5003", false);
    modMan.changeSessionFile("com5003-ced-session.json");
    modMan.waitUntilModulesAreReady();

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    QCOMPARE(modules.count(), 22);

    QCOMPARE(modules[0]->m_moduleId, 1150);
    QCOMPARE(modules[0]->m_module->getVeinModuleName(), "StatusModule1");
    QCOMPARE(modules[0]->m_module->getSCPIModuleName(), "DEV1");

    QCOMPARE(modules[1]->m_moduleId, 1000);
    QCOMPARE(modules[1]->m_module->getVeinModuleName(), "ModeModule1");
    QCOMPARE(modules[1]->m_module->getSCPIModuleName(), "MOD1");

    QCOMPARE(modules[2]->m_moduleId, 1020);
    QCOMPARE(modules[2]->m_module->getVeinModuleName(), "RangeModule1");
    QCOMPARE(modules[2]->m_module->getSCPIModuleName(), "RNG1");

    QCOMPARE(modules[3]->m_moduleId, 1030);
    QCOMPARE(modules[3]->m_module->getVeinModuleName(), "SampleModule1");
    QCOMPARE(modules[3]->m_module->getSCPIModuleName(), "SAM1");

    QCOMPARE(modules[4]->m_moduleId, 1040);
    QCOMPARE(modules[4]->m_module->getVeinModuleName(), "RMSModule1");
    QCOMPARE(modules[4]->m_module->getSCPIModuleName(), "RMS1");

    QCOMPARE(modules[5]->m_moduleId, 1050);
    QCOMPARE(modules[5]->m_module->getVeinModuleName(), "DFTModule1");
    QCOMPARE(modules[5]->m_module->getSCPIModuleName(), "DFT1");

    QCOMPARE(modules[6]->m_moduleId, 1060);
    QCOMPARE(modules[6]->m_module->getVeinModuleName(), "FFTModule1");
    QCOMPARE(modules[6]->m_module->getSCPIModuleName(), "FFT1");

    QCOMPARE(modules[7]->m_moduleId, 1070);
    QCOMPARE(modules[7]->m_module->getVeinModuleName(), "POWER1Module1");
    QCOMPARE(modules[7]->m_module->getSCPIModuleName(), "POW1");

    QCOMPARE(modules[8]->m_moduleId, 1071);
    QCOMPARE(modules[8]->m_module->getVeinModuleName(), "POWER1Module2");
    QCOMPARE(modules[8]->m_module->getSCPIModuleName(), "POW2");

    QCOMPARE(modules[9]->m_moduleId, 1072);
    QCOMPARE(modules[9]->m_module->getVeinModuleName(), "POWER1Module3");
    QCOMPARE(modules[9]->m_module->getSCPIModuleName(), "POW3");

    QCOMPARE(modules[10]->m_moduleId, 1074);
    QCOMPARE(modules[10]->m_module->getVeinModuleName(), "POWER1Module4");
    QCOMPARE(modules[10]->m_module->getSCPIModuleName(), "POW4");

    QCOMPARE(modules[11]->m_moduleId, 1076);
    QCOMPARE(modules[11]->m_module->getVeinModuleName(), "POWER1Module5");
    QCOMPARE(modules[11]->m_module->getSCPIModuleName(), "POW5");

    QCOMPARE(modules[12]->m_moduleId, 1090);
    QCOMPARE(modules[12]->m_module->getVeinModuleName(), "POWER2Module1");
    QCOMPARE(modules[12]->m_module->getSCPIModuleName(), "PW21");

    QCOMPARE(modules[13]->m_moduleId, 1110);
    QCOMPARE(modules[13]->m_module->getVeinModuleName(), "THDNModule1");
    QCOMPARE(modules[13]->m_module->getSCPIModuleName(), "THD1");

    QCOMPARE(modules[14]->m_moduleId, 1111);
    QCOMPARE(modules[14]->m_module->getVeinModuleName(), "THDNModule2");
    QCOMPARE(modules[14]->m_module->getSCPIModuleName(), "THD2");

    QCOMPARE(modules[15]->m_moduleId, 1120);
    QCOMPARE(modules[15]->m_module->getVeinModuleName(), "OSCIModule1");
    QCOMPARE(modules[15]->m_module->getSCPIModuleName(), "OSC1");

    QCOMPARE(modules[16]->m_moduleId, 1130);
    QCOMPARE(modules[16]->m_module->getVeinModuleName(), "SEC1Module1");
    QCOMPARE(modules[16]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[17]->m_moduleId, 1131);
    QCOMPARE(modules[17]->m_module->getVeinModuleName(), "SEC1Module2");
    QCOMPARE(modules[17]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[18]->m_moduleId, 1140);
    QCOMPARE(modules[18]->m_module->getVeinModuleName(), "LambdaModule1");
    QCOMPARE(modules[18]->m_module->getSCPIModuleName(), "LAM1");

    QCOMPARE(modules[19]->m_moduleId, 1300);
    QCOMPARE(modules[19]->m_module->getVeinModuleName(), "SourceModule1");
    QCOMPARE(modules[19]->m_module->getSCPIModuleName(), "SRC1");

    QCOMPARE(modules[20]->m_moduleId, 1400);
    QCOMPARE(modules[20]->m_module->getVeinModuleName(), "BleModule1");
    QCOMPARE(modules[20]->m_module->getSCPIModuleName(), "BLE1");

    QCOMPARE(modules[21]->m_moduleId, 9999);
    QCOMPARE(modules[21]->m_module->getVeinModuleName(), "SCPIModule1");
    QCOMPARE(modules[21]->m_module->getSCPIModuleName(), "SCP1");

    modMan.destroyModulesAndWaitUntilAllShutdown();
    // ------ end auto generated code ----
}

void test_modman_regression_all_sessions::regressionCom5003Meas()
{
    // ------ Start auto generated code for com5003-meas-session.json ----
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("com5003", false);
    modMan.changeSessionFile("com5003-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    QCOMPARE(modules.count(), 25);

    QCOMPARE(modules[0]->m_moduleId, 1150);
    QCOMPARE(modules[0]->m_module->getVeinModuleName(), "StatusModule1");
    QCOMPARE(modules[0]->m_module->getSCPIModuleName(), "DEV1");

    QCOMPARE(modules[1]->m_moduleId, 1000);
    QCOMPARE(modules[1]->m_module->getVeinModuleName(), "ModeModule1");
    QCOMPARE(modules[1]->m_module->getSCPIModuleName(), "MOD1");

    QCOMPARE(modules[2]->m_moduleId, 1020);
    QCOMPARE(modules[2]->m_module->getVeinModuleName(), "RangeModule1");
    QCOMPARE(modules[2]->m_module->getSCPIModuleName(), "RNG1");

    QCOMPARE(modules[3]->m_moduleId, 1030);
    QCOMPARE(modules[3]->m_module->getVeinModuleName(), "SampleModule1");
    QCOMPARE(modules[3]->m_module->getSCPIModuleName(), "SAM1");

    QCOMPARE(modules[4]->m_moduleId, 1040);
    QCOMPARE(modules[4]->m_module->getVeinModuleName(), "RMSModule1");
    QCOMPARE(modules[4]->m_module->getSCPIModuleName(), "RMS1");

    QCOMPARE(modules[5]->m_moduleId, 1050);
    QCOMPARE(modules[5]->m_module->getVeinModuleName(), "DFTModule1");
    QCOMPARE(modules[5]->m_module->getSCPIModuleName(), "DFT1");

    QCOMPARE(modules[6]->m_moduleId, 1060);
    QCOMPARE(modules[6]->m_module->getVeinModuleName(), "FFTModule1");
    QCOMPARE(modules[6]->m_module->getSCPIModuleName(), "FFT1");

    QCOMPARE(modules[7]->m_moduleId, 1070);
    QCOMPARE(modules[7]->m_module->getVeinModuleName(), "POWER1Module1");
    QCOMPARE(modules[7]->m_module->getSCPIModuleName(), "POW1");

    QCOMPARE(modules[8]->m_moduleId, 1071);
    QCOMPARE(modules[8]->m_module->getVeinModuleName(), "POWER1Module2");
    QCOMPARE(modules[8]->m_module->getSCPIModuleName(), "POW2");

    QCOMPARE(modules[9]->m_moduleId, 1072);
    QCOMPARE(modules[9]->m_module->getVeinModuleName(), "POWER1Module3");
    QCOMPARE(modules[9]->m_module->getSCPIModuleName(), "POW3");

    QCOMPARE(modules[10]->m_moduleId, 1073);
    QCOMPARE(modules[10]->m_module->getVeinModuleName(), "POWER1Module4");
    QCOMPARE(modules[10]->m_module->getSCPIModuleName(), "POW4");

    QCOMPARE(modules[11]->m_moduleId, 1074);
    QCOMPARE(modules[11]->m_module->getVeinModuleName(), "POWER1Module5");
    QCOMPARE(modules[11]->m_module->getSCPIModuleName(), "POW5");

    QCOMPARE(modules[12]->m_moduleId, 1076);
    QCOMPARE(modules[12]->m_module->getVeinModuleName(), "POWER1Module6");
    QCOMPARE(modules[12]->m_module->getSCPIModuleName(), "POW6");

    QCOMPARE(modules[13]->m_moduleId, 1100);
    QCOMPARE(modules[13]->m_module->getVeinModuleName(), "Power3Module1");
    QCOMPARE(modules[13]->m_module->getSCPIModuleName(), "PW31");

    QCOMPARE(modules[14]->m_moduleId, 1110);
    QCOMPARE(modules[14]->m_module->getVeinModuleName(), "THDNModule1");
    QCOMPARE(modules[14]->m_module->getSCPIModuleName(), "THD1");

    QCOMPARE(modules[15]->m_moduleId, 1111);
    QCOMPARE(modules[15]->m_module->getVeinModuleName(), "THDNModule2");
    QCOMPARE(modules[15]->m_module->getSCPIModuleName(), "THD2");

    QCOMPARE(modules[16]->m_moduleId, 1120);
    QCOMPARE(modules[16]->m_module->getVeinModuleName(), "OSCIModule1");
    QCOMPARE(modules[16]->m_module->getSCPIModuleName(), "OSC1");

    QCOMPARE(modules[17]->m_moduleId, 1130);
    QCOMPARE(modules[17]->m_module->getVeinModuleName(), "SEC1Module1");
    QCOMPARE(modules[17]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[18]->m_moduleId, 1131);
    QCOMPARE(modules[18]->m_module->getVeinModuleName(), "SEC1Module2");
    QCOMPARE(modules[18]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[19]->m_moduleId, 1140);
    QCOMPARE(modules[19]->m_module->getVeinModuleName(), "LambdaModule1");
    QCOMPARE(modules[19]->m_module->getSCPIModuleName(), "LAM1");

    QCOMPARE(modules[20]->m_moduleId, 1190);
    QCOMPARE(modules[20]->m_module->getVeinModuleName(), "AdjustmentModule1");
    QCOMPARE(modules[20]->m_module->getSCPIModuleName(), "ADJ1");

    QCOMPARE(modules[21]->m_moduleId, 1200);
    QCOMPARE(modules[21]->m_module->getVeinModuleName(), "SEM1Module1");
    QCOMPARE(modules[21]->m_module->getSCPIModuleName(), "EM01");

    QCOMPARE(modules[22]->m_moduleId, 1300);
    QCOMPARE(modules[22]->m_module->getVeinModuleName(), "SourceModule1");
    QCOMPARE(modules[22]->m_module->getSCPIModuleName(), "SRC1");

    QCOMPARE(modules[23]->m_moduleId, 1400);
    QCOMPARE(modules[23]->m_module->getVeinModuleName(), "BleModule1");
    QCOMPARE(modules[23]->m_module->getSCPIModuleName(), "BLE1");

    QCOMPARE(modules[24]->m_moduleId, 9999);
    QCOMPARE(modules[24]->m_module->getVeinModuleName(), "SCPIModule1");
    QCOMPARE(modules[24]->m_module->getSCPIModuleName(), "SCP1");

    modMan.destroyModulesAndWaitUntilAllShutdown();
    // ------ end auto generated code ----
}

void test_modman_regression_all_sessions::regressionCom5003PerPhase()
{
    // ------ Start auto generated code for com5003-perphase-session.json ----
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("com5003", false);
    modMan.changeSessionFile("com5003-perphase-session.json");
    modMan.waitUntilModulesAreReady();

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    QCOMPARE(modules.count(), 26);

    QCOMPARE(modules[0]->m_moduleId, 1150);
    QCOMPARE(modules[0]->m_module->getVeinModuleName(), "StatusModule1");
    QCOMPARE(modules[0]->m_module->getSCPIModuleName(), "DEV1");

    QCOMPARE(modules[1]->m_moduleId, 1000);
    QCOMPARE(modules[1]->m_module->getVeinModuleName(), "ModeModule1");
    QCOMPARE(modules[1]->m_module->getSCPIModuleName(), "MOD1");

    QCOMPARE(modules[2]->m_moduleId, 1020);
    QCOMPARE(modules[2]->m_module->getVeinModuleName(), "RangeModule1");
    QCOMPARE(modules[2]->m_module->getSCPIModuleName(), "RNG1");

    QCOMPARE(modules[3]->m_moduleId, 1030);
    QCOMPARE(modules[3]->m_module->getVeinModuleName(), "SampleModule1");
    QCOMPARE(modules[3]->m_module->getSCPIModuleName(), "SAM1");

    QCOMPARE(modules[4]->m_moduleId, 1040);
    QCOMPARE(modules[4]->m_module->getVeinModuleName(), "RMSModule1");
    QCOMPARE(modules[4]->m_module->getSCPIModuleName(), "RMS1");

    QCOMPARE(modules[5]->m_moduleId, 1050);
    QCOMPARE(modules[5]->m_module->getVeinModuleName(), "DFTModule1");
    QCOMPARE(modules[5]->m_module->getSCPIModuleName(), "DFT1");

    QCOMPARE(modules[6]->m_moduleId, 1060);
    QCOMPARE(modules[6]->m_module->getVeinModuleName(), "FFTModule1");
    QCOMPARE(modules[6]->m_module->getSCPIModuleName(), "FFT1");

    QCOMPARE(modules[7]->m_moduleId, 1070);
    QCOMPARE(modules[7]->m_module->getVeinModuleName(), "POWER1Module1");
    QCOMPARE(modules[7]->m_module->getSCPIModuleName(), "POW1");

    QCOMPARE(modules[8]->m_moduleId, 1071);
    QCOMPARE(modules[8]->m_module->getVeinModuleName(), "POWER1Module2");
    QCOMPARE(modules[8]->m_module->getSCPIModuleName(), "POW2");

    QCOMPARE(modules[9]->m_moduleId, 1072);
    QCOMPARE(modules[9]->m_module->getVeinModuleName(), "POWER1Module3");
    QCOMPARE(modules[9]->m_module->getSCPIModuleName(), "POW3");

    QCOMPARE(modules[10]->m_moduleId, 1073);
    QCOMPARE(modules[10]->m_module->getVeinModuleName(), "POWER1Module4");
    QCOMPARE(modules[10]->m_module->getSCPIModuleName(), "POW4");

    QCOMPARE(modules[11]->m_moduleId, 1074);
    QCOMPARE(modules[11]->m_module->getVeinModuleName(), "POWER1Module5");
    QCOMPARE(modules[11]->m_module->getSCPIModuleName(), "POW5");

    QCOMPARE(modules[12]->m_moduleId, 1075);
    QCOMPARE(modules[12]->m_module->getVeinModuleName(), "POWER1Module6");
    QCOMPARE(modules[12]->m_module->getSCPIModuleName(), "POW6");

    QCOMPARE(modules[13]->m_moduleId, 1076);
    QCOMPARE(modules[13]->m_module->getVeinModuleName(), "POWER1Module7");
    QCOMPARE(modules[13]->m_module->getSCPIModuleName(), "POW7");

    QCOMPARE(modules[14]->m_moduleId, 1100);
    QCOMPARE(modules[14]->m_module->getVeinModuleName(), "Power3Module1");
    QCOMPARE(modules[14]->m_module->getSCPIModuleName(), "PW31");

    QCOMPARE(modules[15]->m_moduleId, 1110);
    QCOMPARE(modules[15]->m_module->getVeinModuleName(), "THDNModule1");
    QCOMPARE(modules[15]->m_module->getSCPIModuleName(), "THD1");

    QCOMPARE(modules[16]->m_moduleId, 1111);
    QCOMPARE(modules[16]->m_module->getVeinModuleName(), "THDNModule2");
    QCOMPARE(modules[16]->m_module->getSCPIModuleName(), "THD2");

    QCOMPARE(modules[17]->m_moduleId, 1120);
    QCOMPARE(modules[17]->m_module->getVeinModuleName(), "OSCIModule1");
    QCOMPARE(modules[17]->m_module->getSCPIModuleName(), "OSC1");

    QCOMPARE(modules[18]->m_moduleId, 1130);
    QCOMPARE(modules[18]->m_module->getVeinModuleName(), "SEC1Module1");
    QCOMPARE(modules[18]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[19]->m_moduleId, 1131);
    QCOMPARE(modules[19]->m_module->getVeinModuleName(), "SEC1Module2");
    QCOMPARE(modules[19]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[20]->m_moduleId, 1140);
    QCOMPARE(modules[20]->m_module->getVeinModuleName(), "LambdaModule1");
    QCOMPARE(modules[20]->m_module->getSCPIModuleName(), "LAM1");

    QCOMPARE(modules[21]->m_moduleId, 1190);
    QCOMPARE(modules[21]->m_module->getVeinModuleName(), "AdjustmentModule1");
    QCOMPARE(modules[21]->m_module->getSCPIModuleName(), "ADJ1");

    QCOMPARE(modules[22]->m_moduleId, 1200);
    QCOMPARE(modules[22]->m_module->getVeinModuleName(), "SEM1Module1");
    QCOMPARE(modules[22]->m_module->getSCPIModuleName(), "EM01");

    QCOMPARE(modules[23]->m_moduleId, 1300);
    QCOMPARE(modules[23]->m_module->getVeinModuleName(), "SourceModule1");
    QCOMPARE(modules[23]->m_module->getSCPIModuleName(), "SRC1");

    QCOMPARE(modules[24]->m_moduleId, 1400);
    QCOMPARE(modules[24]->m_module->getVeinModuleName(), "BleModule1");
    QCOMPARE(modules[24]->m_module->getSCPIModuleName(), "BLE1");

    QCOMPARE(modules[25]->m_moduleId, 9999);
    QCOMPARE(modules[25]->m_module->getVeinModuleName(), "SCPIModule1");
    QCOMPARE(modules[25]->m_module->getSCPIModuleName(), "SCP1");

    modMan.destroyModulesAndWaitUntilAllShutdown();
    // ------ end auto generated code ----
}

void test_modman_regression_all_sessions::regressionCom5003Ref()
{
    // ------ Start auto generated code for com5003-ref-session.json ----
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("com5003", false);
    modMan.changeSessionFile("com5003-ref-session.json");
    modMan.waitUntilModulesAreReady();

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    QCOMPARE(modules.count(), 9);

    QCOMPARE(modules[0]->m_moduleId, 1150);
    QCOMPARE(modules[0]->m_module->getVeinModuleName(), "StatusModule1");
    QCOMPARE(modules[0]->m_module->getSCPIModuleName(), "DEV1");

    QCOMPARE(modules[1]->m_moduleId, 1000);
    QCOMPARE(modules[1]->m_module->getVeinModuleName(), "ModeModule1");
    QCOMPARE(modules[1]->m_module->getSCPIModuleName(), "MOD1");

    QCOMPARE(modules[2]->m_moduleId, 1001);
    QCOMPARE(modules[2]->m_module->getVeinModuleName(), "REFERENCEModule1");
    QCOMPARE(modules[2]->m_module->getSCPIModuleName(), "REF1");

    QCOMPARE(modules[3]->m_moduleId, 1020);
    QCOMPARE(modules[3]->m_module->getVeinModuleName(), "RangeModule1");
    QCOMPARE(modules[3]->m_module->getSCPIModuleName(), "RNG1");

    QCOMPARE(modules[4]->m_moduleId, 1050);
    QCOMPARE(modules[4]->m_module->getVeinModuleName(), "DFTModule1");
    QCOMPARE(modules[4]->m_module->getSCPIModuleName(), "DFT1");

    QCOMPARE(modules[5]->m_moduleId, 1070);
    QCOMPARE(modules[5]->m_module->getVeinModuleName(), "POWER1Module1");
    QCOMPARE(modules[5]->m_module->getSCPIModuleName(), "POW1");

    QCOMPARE(modules[6]->m_moduleId, 1130);
    QCOMPARE(modules[6]->m_module->getVeinModuleName(), "SEC1Module1");
    QCOMPARE(modules[6]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[7]->m_moduleId, 1400);
    QCOMPARE(modules[7]->m_module->getVeinModuleName(), "BleModule1");
    QCOMPARE(modules[7]->m_module->getSCPIModuleName(), "BLE1");

    QCOMPARE(modules[8]->m_moduleId, 9999);
    QCOMPARE(modules[8]->m_module->getVeinModuleName(), "SCPIModule1");
    QCOMPARE(modules[8]->m_module->getSCPIModuleName(), "SCP1");

    modMan.destroyModulesAndWaitUntilAllShutdown();
    // ------ end auto generated code ----
}

void test_modman_regression_all_sessions::regressionMt310s2Ced()
{
    // ------ Start auto generated code for mt310s2-ced-session.json ----
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-ced-session.json");
    modMan.waitUntilModulesAreReady();

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    QCOMPARE(modules.count(), 20);

    QCOMPARE(modules[0]->m_moduleId, 1150);
    QCOMPARE(modules[0]->m_module->getVeinModuleName(), "StatusModule1");
    QCOMPARE(modules[0]->m_module->getSCPIModuleName(), "DEV1");

    QCOMPARE(modules[1]->m_moduleId, 1000);
    QCOMPARE(modules[1]->m_module->getVeinModuleName(), "ModeModule1");
    QCOMPARE(modules[1]->m_module->getSCPIModuleName(), "MOD1");

    QCOMPARE(modules[2]->m_moduleId, 1020);
    QCOMPARE(modules[2]->m_module->getVeinModuleName(), "RangeModule1");
    QCOMPARE(modules[2]->m_module->getSCPIModuleName(), "RNG1");

    QCOMPARE(modules[3]->m_moduleId, 1030);
    QCOMPARE(modules[3]->m_module->getVeinModuleName(), "SampleModule1");
    QCOMPARE(modules[3]->m_module->getSCPIModuleName(), "SAM1");

    QCOMPARE(modules[4]->m_moduleId, 1040);
    QCOMPARE(modules[4]->m_module->getVeinModuleName(), "RMSModule1");
    QCOMPARE(modules[4]->m_module->getSCPIModuleName(), "RMS1");

    QCOMPARE(modules[5]->m_moduleId, 1050);
    QCOMPARE(modules[5]->m_module->getVeinModuleName(), "DFTModule1");
    QCOMPARE(modules[5]->m_module->getSCPIModuleName(), "DFT1");

    QCOMPARE(modules[6]->m_moduleId, 1060);
    QCOMPARE(modules[6]->m_module->getVeinModuleName(), "FFTModule1");
    QCOMPARE(modules[6]->m_module->getSCPIModuleName(), "FFT1");

    QCOMPARE(modules[7]->m_moduleId, 1070);
    QCOMPARE(modules[7]->m_module->getVeinModuleName(), "POWER1Module1");
    QCOMPARE(modules[7]->m_module->getSCPIModuleName(), "POW1");

    QCOMPARE(modules[8]->m_moduleId, 1071);
    QCOMPARE(modules[8]->m_module->getVeinModuleName(), "POWER1Module2");
    QCOMPARE(modules[8]->m_module->getSCPIModuleName(), "POW2");

    QCOMPARE(modules[9]->m_moduleId, 1072);
    QCOMPARE(modules[9]->m_module->getVeinModuleName(), "POWER1Module3");
    QCOMPARE(modules[9]->m_module->getSCPIModuleName(), "POW3");

    QCOMPARE(modules[10]->m_moduleId, 1074);
    QCOMPARE(modules[10]->m_module->getVeinModuleName(), "POWER1Module4");
    QCOMPARE(modules[10]->m_module->getSCPIModuleName(), "POW4");

    QCOMPARE(modules[11]->m_moduleId, 1076);
    QCOMPARE(modules[11]->m_module->getVeinModuleName(), "POWER1Module5");
    QCOMPARE(modules[11]->m_module->getSCPIModuleName(), "POW5");

    QCOMPARE(modules[12]->m_moduleId, 1090);
    QCOMPARE(modules[12]->m_module->getVeinModuleName(), "POWER2Module1");
    QCOMPARE(modules[12]->m_module->getSCPIModuleName(), "PW21");

    QCOMPARE(modules[13]->m_moduleId, 1110);
    QCOMPARE(modules[13]->m_module->getVeinModuleName(), "THDNModule1");
    QCOMPARE(modules[13]->m_module->getSCPIModuleName(), "THD1");

    QCOMPARE(modules[14]->m_moduleId, 1111);
    QCOMPARE(modules[14]->m_module->getVeinModuleName(), "THDNModule2");
    QCOMPARE(modules[14]->m_module->getSCPIModuleName(), "THD2");

    QCOMPARE(modules[15]->m_moduleId, 1120);
    QCOMPARE(modules[15]->m_module->getVeinModuleName(), "OSCIModule1");
    QCOMPARE(modules[15]->m_module->getSCPIModuleName(), "OSC1");

    QCOMPARE(modules[16]->m_moduleId, 1130);
    QCOMPARE(modules[16]->m_module->getVeinModuleName(), "SEC1Module1");
    QCOMPARE(modules[16]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[17]->m_moduleId, 1140);
    QCOMPARE(modules[17]->m_module->getVeinModuleName(), "LambdaModule1");
    QCOMPARE(modules[17]->m_module->getSCPIModuleName(), "LAM1");

    QCOMPARE(modules[18]->m_moduleId, 1300);
    QCOMPARE(modules[18]->m_module->getVeinModuleName(), "SourceModule1");
    QCOMPARE(modules[18]->m_module->getSCPIModuleName(), "SRC1");

    QCOMPARE(modules[19]->m_moduleId, 9999);
    QCOMPARE(modules[19]->m_module->getVeinModuleName(), "SCPIModule1");
    QCOMPARE(modules[19]->m_module->getSCPIModuleName(), "SCP1");

    modMan.destroyModulesAndWaitUntilAllShutdown();
    // ------ end auto generated code ----
}

void test_modman_regression_all_sessions::regressionMt310s2Dc()
{
    // ------ Start auto generated code for mt310s2-dc-session.json ----
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-dc-session.json");
    modMan.waitUntilModulesAreReady();

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    QCOMPARE(modules.count(), 22);

    QCOMPARE(modules[0]->m_moduleId, 1150);
    QCOMPARE(modules[0]->m_module->getVeinModuleName(), "StatusModule1");
    QCOMPARE(modules[0]->m_module->getSCPIModuleName(), "DEV1");

    QCOMPARE(modules[1]->m_moduleId, 1000);
    QCOMPARE(modules[1]->m_module->getVeinModuleName(), "ModeModule1");
    QCOMPARE(modules[1]->m_module->getSCPIModuleName(), "MOD1");

    QCOMPARE(modules[2]->m_moduleId, 1020);
    QCOMPARE(modules[2]->m_module->getVeinModuleName(), "RangeModule1");
    QCOMPARE(modules[2]->m_module->getSCPIModuleName(), "RNG1");

    QCOMPARE(modules[3]->m_moduleId, 1030);
    QCOMPARE(modules[3]->m_module->getVeinModuleName(), "SampleModule1");
    QCOMPARE(modules[3]->m_module->getSCPIModuleName(), "SAM1");

    QCOMPARE(modules[4]->m_moduleId, 1040);
    QCOMPARE(modules[4]->m_module->getVeinModuleName(), "RMSModule1");
    QCOMPARE(modules[4]->m_module->getSCPIModuleName(), "RMS1");

    QCOMPARE(modules[5]->m_moduleId, 1050);
    QCOMPARE(modules[5]->m_module->getVeinModuleName(), "DFTModule1");
    QCOMPARE(modules[5]->m_module->getSCPIModuleName(), "DFT1");

    QCOMPARE(modules[6]->m_moduleId, 1060);
    QCOMPARE(modules[6]->m_module->getVeinModuleName(), "FFTModule1");
    QCOMPARE(modules[6]->m_module->getSCPIModuleName(), "FFT1");

    QCOMPARE(modules[7]->m_moduleId, 1070);
    QCOMPARE(modules[7]->m_module->getVeinModuleName(), "POWER1Module1");
    QCOMPARE(modules[7]->m_module->getSCPIModuleName(), "POW1");

    QCOMPARE(modules[8]->m_moduleId, 1071);
    QCOMPARE(modules[8]->m_module->getVeinModuleName(), "POWER1Module2");
    QCOMPARE(modules[8]->m_module->getSCPIModuleName(), "POW2");

    QCOMPARE(modules[9]->m_moduleId, 1072);
    QCOMPARE(modules[9]->m_module->getVeinModuleName(), "POWER1Module3");
    QCOMPARE(modules[9]->m_module->getSCPIModuleName(), "POW3");

    QCOMPARE(modules[10]->m_moduleId, 1073);
    QCOMPARE(modules[10]->m_module->getVeinModuleName(), "POWER1Module4");
    QCOMPARE(modules[10]->m_module->getSCPIModuleName(), "POW4");

    QCOMPARE(modules[11]->m_moduleId, 1076);
    QCOMPARE(modules[11]->m_module->getVeinModuleName(), "POWER1Module5");
    QCOMPARE(modules[11]->m_module->getSCPIModuleName(), "POW5");

    QCOMPARE(modules[12]->m_moduleId, 1110);
    QCOMPARE(modules[12]->m_module->getVeinModuleName(), "THDNModule1");
    QCOMPARE(modules[12]->m_module->getSCPIModuleName(), "THD1");

    QCOMPARE(modules[13]->m_moduleId, 1111);
    QCOMPARE(modules[13]->m_module->getVeinModuleName(), "THDNModule2");
    QCOMPARE(modules[13]->m_module->getSCPIModuleName(), "THD2");

    QCOMPARE(modules[14]->m_moduleId, 1120);
    QCOMPARE(modules[14]->m_module->getVeinModuleName(), "OSCIModule1");
    QCOMPARE(modules[14]->m_module->getSCPIModuleName(), "OSC1");

    QCOMPARE(modules[15]->m_moduleId, 1130);
    QCOMPARE(modules[15]->m_module->getVeinModuleName(), "SEC1Module1");
    QCOMPARE(modules[15]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[16]->m_moduleId, 1140);
    QCOMPARE(modules[16]->m_module->getVeinModuleName(), "LambdaModule1");
    QCOMPARE(modules[16]->m_module->getSCPIModuleName(), "LAM1");

    QCOMPARE(modules[17]->m_moduleId, 1190);
    QCOMPARE(modules[17]->m_module->getVeinModuleName(), "AdjustmentModule1");
    QCOMPARE(modules[17]->m_module->getSCPIModuleName(), "ADJ1");

    QCOMPARE(modules[18]->m_moduleId, 1200);
    QCOMPARE(modules[18]->m_module->getVeinModuleName(), "SEM1Module1");
    QCOMPARE(modules[18]->m_module->getSCPIModuleName(), "EM01");

    QCOMPARE(modules[19]->m_moduleId, 1300);
    QCOMPARE(modules[19]->m_module->getVeinModuleName(), "SourceModule1");
    QCOMPARE(modules[19]->m_module->getSCPIModuleName(), "SRC1");

    QCOMPARE(modules[20]->m_moduleId, 1400);
    QCOMPARE(modules[20]->m_module->getVeinModuleName(), "BleModule1");
    QCOMPARE(modules[20]->m_module->getSCPIModuleName(), "BLE1");

    QCOMPARE(modules[21]->m_moduleId, 9999);
    QCOMPARE(modules[21]->m_module->getVeinModuleName(), "SCPIModule1");
    QCOMPARE(modules[21]->m_module->getSCPIModuleName(), "SCP1");

    modMan.destroyModulesAndWaitUntilAllShutdown();
    // ------ end auto generated code ----
}

void test_modman_regression_all_sessions::regressionMt310s2EmobAc()
{
    // ------ Start auto generated code for mt310s2-emob-session-ac.json ----
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-emob-session-ac.json");
    modMan.waitUntilModulesAreReady();

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    QCOMPARE(modules.count(), 21);

    QCOMPARE(modules[0]->m_moduleId, 1150);
    QCOMPARE(modules[0]->m_module->getVeinModuleName(), "StatusModule1");
    QCOMPARE(modules[0]->m_module->getSCPIModuleName(), "DEV1");

    QCOMPARE(modules[1]->m_moduleId, 1000);
    QCOMPARE(modules[1]->m_module->getVeinModuleName(), "ModeModule1");
    QCOMPARE(modules[1]->m_module->getSCPIModuleName(), "MOD1");

    QCOMPARE(modules[2]->m_moduleId, 1020);
    QCOMPARE(modules[2]->m_module->getVeinModuleName(), "RangeModule1");
    QCOMPARE(modules[2]->m_module->getSCPIModuleName(), "RNG1");

    QCOMPARE(modules[3]->m_moduleId, 1030);
    QCOMPARE(modules[3]->m_module->getVeinModuleName(), "SampleModule1");
    QCOMPARE(modules[3]->m_module->getSCPIModuleName(), "SAM1");

    QCOMPARE(modules[4]->m_moduleId, 1040);
    QCOMPARE(modules[4]->m_module->getVeinModuleName(), "RMSModule1");
    QCOMPARE(modules[4]->m_module->getSCPIModuleName(), "RMS1");

    QCOMPARE(modules[5]->m_moduleId, 1050);
    QCOMPARE(modules[5]->m_module->getVeinModuleName(), "DFTModule1");
    QCOMPARE(modules[5]->m_module->getSCPIModuleName(), "DFT1");

    QCOMPARE(modules[6]->m_moduleId, 1060);
    QCOMPARE(modules[6]->m_module->getVeinModuleName(), "FFTModule1");
    QCOMPARE(modules[6]->m_module->getSCPIModuleName(), "FFT1");

    QCOMPARE(modules[7]->m_moduleId, 1070);
    QCOMPARE(modules[7]->m_module->getVeinModuleName(), "POWER1Module1");
    QCOMPARE(modules[7]->m_module->getSCPIModuleName(), "POW1");

    QCOMPARE(modules[8]->m_moduleId, 1071);
    QCOMPARE(modules[8]->m_module->getVeinModuleName(), "POWER1Module2");
    QCOMPARE(modules[8]->m_module->getSCPIModuleName(), "POW2");

    QCOMPARE(modules[9]->m_moduleId, 1072);
    QCOMPARE(modules[9]->m_module->getVeinModuleName(), "POWER1Module3");
    QCOMPARE(modules[9]->m_module->getSCPIModuleName(), "POW3");

    QCOMPARE(modules[10]->m_moduleId, 1076);
    QCOMPARE(modules[10]->m_module->getVeinModuleName(), "POWER1Module5");
    QCOMPARE(modules[10]->m_module->getSCPIModuleName(), "POW5");

    QCOMPARE(modules[11]->m_moduleId, 1110);
    QCOMPARE(modules[11]->m_module->getVeinModuleName(), "THDNModule1");
    QCOMPARE(modules[11]->m_module->getSCPIModuleName(), "THD1");

    QCOMPARE(modules[12]->m_moduleId, 1111);
    QCOMPARE(modules[12]->m_module->getVeinModuleName(), "THDNModule2");
    QCOMPARE(modules[12]->m_module->getSCPIModuleName(), "THD2");

    QCOMPARE(modules[13]->m_moduleId, 1120);
    QCOMPARE(modules[13]->m_module->getVeinModuleName(), "OSCIModule1");
    QCOMPARE(modules[13]->m_module->getSCPIModuleName(), "OSC1");

    QCOMPARE(modules[14]->m_moduleId, 1130);
    QCOMPARE(modules[14]->m_module->getVeinModuleName(), "SEC1Module1");
    QCOMPARE(modules[14]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[15]->m_moduleId, 1140);
    QCOMPARE(modules[15]->m_module->getVeinModuleName(), "LambdaModule1");
    QCOMPARE(modules[15]->m_module->getSCPIModuleName(), "LAM1");

    QCOMPARE(modules[16]->m_moduleId, 1190);
    QCOMPARE(modules[16]->m_module->getVeinModuleName(), "AdjustmentModule1");
    QCOMPARE(modules[16]->m_module->getSCPIModuleName(), "ADJ1");

    QCOMPARE(modules[17]->m_moduleId, 1200);
    QCOMPARE(modules[17]->m_module->getVeinModuleName(), "SEM1Module1");
    QCOMPARE(modules[17]->m_module->getSCPIModuleName(), "EM01");

    QCOMPARE(modules[18]->m_moduleId, 1300);
    QCOMPARE(modules[18]->m_module->getVeinModuleName(), "SourceModule1");
    QCOMPARE(modules[18]->m_module->getSCPIModuleName(), "SRC1");

    QCOMPARE(modules[19]->m_moduleId, 1400);
    QCOMPARE(modules[19]->m_module->getVeinModuleName(), "BleModule1");
    QCOMPARE(modules[19]->m_module->getSCPIModuleName(), "BLE1");

    QCOMPARE(modules[20]->m_moduleId, 9999);
    QCOMPARE(modules[20]->m_module->getVeinModuleName(), "SCPIModule1");
    QCOMPARE(modules[20]->m_module->getSCPIModuleName(), "SCP1");

    modMan.destroyModulesAndWaitUntilAllShutdown();
    // ------ end auto generated code ----
}

void test_modman_regression_all_sessions::regressionMt310s2EmobDc()
{
    // ------ Start auto generated code for mt310s2-emob-session-dc.json ----
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-emob-session-dc.json");
    modMan.waitUntilModulesAreReady();

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    QCOMPARE(modules.count(), 17);

    QCOMPARE(modules[0]->m_moduleId, 1150);
    QCOMPARE(modules[0]->m_module->getVeinModuleName(), "StatusModule1");
    QCOMPARE(modules[0]->m_module->getSCPIModuleName(), "DEV1");

    QCOMPARE(modules[1]->m_moduleId, 1000);
    QCOMPARE(modules[1]->m_module->getVeinModuleName(), "ModeModule1");
    QCOMPARE(modules[1]->m_module->getSCPIModuleName(), "MOD1");

    QCOMPARE(modules[2]->m_moduleId, 1020);
    QCOMPARE(modules[2]->m_module->getVeinModuleName(), "RangeModule1");
    QCOMPARE(modules[2]->m_module->getSCPIModuleName(), "RNG1");

    QCOMPARE(modules[3]->m_moduleId, 1030);
    QCOMPARE(modules[3]->m_module->getVeinModuleName(), "SampleModule1");
    QCOMPARE(modules[3]->m_module->getSCPIModuleName(), "SAM1");

    QCOMPARE(modules[4]->m_moduleId, 1040);
    QCOMPARE(modules[4]->m_module->getVeinModuleName(), "RMSModule1");
    QCOMPARE(modules[4]->m_module->getSCPIModuleName(), "RMS1");

    QCOMPARE(modules[5]->m_moduleId, 1050);
    QCOMPARE(modules[5]->m_module->getVeinModuleName(), "DFTModule1");
    QCOMPARE(modules[5]->m_module->getSCPIModuleName(), "DFT1");

    QCOMPARE(modules[6]->m_moduleId, 1060);
    QCOMPARE(modules[6]->m_module->getVeinModuleName(), "FFTModule1");
    QCOMPARE(modules[6]->m_module->getSCPIModuleName(), "FFT1");

    QCOMPARE(modules[7]->m_moduleId, 1073);
    QCOMPARE(modules[7]->m_module->getVeinModuleName(), "POWER1Module4");
    QCOMPARE(modules[7]->m_module->getSCPIModuleName(), "POW4");

    QCOMPARE(modules[8]->m_moduleId, 1110);
    QCOMPARE(modules[8]->m_module->getVeinModuleName(), "THDNModule1");
    QCOMPARE(modules[8]->m_module->getSCPIModuleName(), "THD1");

    QCOMPARE(modules[9]->m_moduleId, 1111);
    QCOMPARE(modules[9]->m_module->getVeinModuleName(), "THDNModule2");
    QCOMPARE(modules[9]->m_module->getSCPIModuleName(), "THD2");

    QCOMPARE(modules[10]->m_moduleId, 1120);
    QCOMPARE(modules[10]->m_module->getVeinModuleName(), "OSCIModule1");
    QCOMPARE(modules[10]->m_module->getSCPIModuleName(), "OSC1");

    QCOMPARE(modules[11]->m_moduleId, 1130);
    QCOMPARE(modules[11]->m_module->getVeinModuleName(), "SEC1Module1");
    QCOMPARE(modules[11]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[12]->m_moduleId, 1190);
    QCOMPARE(modules[12]->m_module->getVeinModuleName(), "AdjustmentModule1");
    QCOMPARE(modules[12]->m_module->getSCPIModuleName(), "ADJ1");

    QCOMPARE(modules[13]->m_moduleId, 1200);
    QCOMPARE(modules[13]->m_module->getVeinModuleName(), "SEM1Module1");
    QCOMPARE(modules[13]->m_module->getSCPIModuleName(), "EM01");

    QCOMPARE(modules[14]->m_moduleId, 1300);
    QCOMPARE(modules[14]->m_module->getVeinModuleName(), "SourceModule1");
    QCOMPARE(modules[14]->m_module->getSCPIModuleName(), "SRC1");

    QCOMPARE(modules[15]->m_moduleId, 1400);
    QCOMPARE(modules[15]->m_module->getVeinModuleName(), "BleModule1");
    QCOMPARE(modules[15]->m_module->getSCPIModuleName(), "BLE1");

    QCOMPARE(modules[16]->m_moduleId, 9999);
    QCOMPARE(modules[16]->m_module->getVeinModuleName(), "SCPIModule1");
    QCOMPARE(modules[16]->m_module->getSCPIModuleName(), "SCP1");

    modMan.destroyModulesAndWaitUntilAllShutdown();
    // ------ end auto generated code ----
}

void test_modman_regression_all_sessions::regressionMt310s2Meas()
{
    // ------ Start auto generated code for mt310s2-meas-session.json ----
    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices("mt310s2", false);
    modMan.changeSessionFile("mt310s2-meas-session.json");
    modMan.waitUntilModulesAreReady();

    QList<ZeraModules::ModuleData *> modules = modMan.getModuleList();
    QCOMPARE(modules.count(), 28);

    QCOMPARE(modules[0]->m_moduleId, 1150);
    QCOMPARE(modules[0]->m_module->getVeinModuleName(), "StatusModule1");
    QCOMPARE(modules[0]->m_module->getSCPIModuleName(), "DEV1");

    QCOMPARE(modules[1]->m_moduleId, 1000);
    QCOMPARE(modules[1]->m_module->getVeinModuleName(), "ModeModule1");
    QCOMPARE(modules[1]->m_module->getSCPIModuleName(), "MOD1");

    QCOMPARE(modules[2]->m_moduleId, 1020);
    QCOMPARE(modules[2]->m_module->getVeinModuleName(), "RangeModule1");
    QCOMPARE(modules[2]->m_module->getSCPIModuleName(), "RNG1");

    QCOMPARE(modules[3]->m_moduleId, 1030);
    QCOMPARE(modules[3]->m_module->getVeinModuleName(), "SampleModule1");
    QCOMPARE(modules[3]->m_module->getSCPIModuleName(), "SAM1");

    QCOMPARE(modules[4]->m_moduleId, 1040);
    QCOMPARE(modules[4]->m_module->getVeinModuleName(), "RMSModule1");
    QCOMPARE(modules[4]->m_module->getSCPIModuleName(), "RMS1");

    QCOMPARE(modules[5]->m_moduleId, 1050);
    QCOMPARE(modules[5]->m_module->getVeinModuleName(), "DFTModule1");
    QCOMPARE(modules[5]->m_module->getSCPIModuleName(), "DFT1");

    QCOMPARE(modules[6]->m_moduleId, 1060);
    QCOMPARE(modules[6]->m_module->getVeinModuleName(), "FFTModule1");
    QCOMPARE(modules[6]->m_module->getSCPIModuleName(), "FFT1");

    QCOMPARE(modules[7]->m_moduleId, 1070);
    QCOMPARE(modules[7]->m_module->getVeinModuleName(), "POWER1Module1");
    QCOMPARE(modules[7]->m_module->getSCPIModuleName(), "POW1");

    QCOMPARE(modules[8]->m_moduleId, 1071);
    QCOMPARE(modules[8]->m_module->getVeinModuleName(), "POWER1Module2");
    QCOMPARE(modules[8]->m_module->getSCPIModuleName(), "POW2");

    QCOMPARE(modules[9]->m_moduleId, 1072);
    QCOMPARE(modules[9]->m_module->getVeinModuleName(), "POWER1Module3");
    QCOMPARE(modules[9]->m_module->getSCPIModuleName(), "POW3");

    QCOMPARE(modules[10]->m_moduleId, 1073);
    QCOMPARE(modules[10]->m_module->getVeinModuleName(), "POWER1Module4");
    QCOMPARE(modules[10]->m_module->getSCPIModuleName(), "POW4");

    QCOMPARE(modules[11]->m_moduleId, 1074);
    QCOMPARE(modules[11]->m_module->getVeinModuleName(), "POWER1Module5");
    QCOMPARE(modules[11]->m_module->getSCPIModuleName(), "POW5");

    QCOMPARE(modules[12]->m_moduleId, 1076);
    QCOMPARE(modules[12]->m_module->getVeinModuleName(), "POWER1Module6");
    QCOMPARE(modules[12]->m_module->getSCPIModuleName(), "POW6");

    QCOMPARE(modules[13]->m_moduleId, 1100);
    QCOMPARE(modules[13]->m_module->getVeinModuleName(), "Power3Module1");
    QCOMPARE(modules[13]->m_module->getSCPIModuleName(), "PW31");

    QCOMPARE(modules[14]->m_moduleId, 1110);
    QCOMPARE(modules[14]->m_module->getVeinModuleName(), "THDNModule1");
    QCOMPARE(modules[14]->m_module->getSCPIModuleName(), "THD1");

    QCOMPARE(modules[15]->m_moduleId, 1111);
    QCOMPARE(modules[15]->m_module->getVeinModuleName(), "THDNModule2");
    QCOMPARE(modules[15]->m_module->getSCPIModuleName(), "THD2");

    QCOMPARE(modules[16]->m_moduleId, 1120);
    QCOMPARE(modules[16]->m_module->getVeinModuleName(), "OSCIModule1");
    QCOMPARE(modules[16]->m_module->getSCPIModuleName(), "OSC1");

    QCOMPARE(modules[17]->m_moduleId, 1130);
    QCOMPARE(modules[17]->m_module->getVeinModuleName(), "SEC1Module1");
    QCOMPARE(modules[17]->m_module->getSCPIModuleName(), "EC01");

    QCOMPARE(modules[18]->m_moduleId, 1140);
    QCOMPARE(modules[18]->m_module->getVeinModuleName(), "LambdaModule1");
    QCOMPARE(modules[18]->m_module->getSCPIModuleName(), "LAM1");

    QCOMPARE(modules[19]->m_moduleId, 1160);
    QCOMPARE(modules[19]->m_module->getVeinModuleName(), "Burden1Module1");
    QCOMPARE(modules[19]->m_module->getSCPIModuleName(), "BD11");

    QCOMPARE(modules[20]->m_moduleId, 1161);
    QCOMPARE(modules[20]->m_module->getVeinModuleName(), "Burden1Module2");
    QCOMPARE(modules[20]->m_module->getSCPIModuleName(), "BD12");

    QCOMPARE(modules[21]->m_moduleId, 1170);
    QCOMPARE(modules[21]->m_module->getVeinModuleName(), "Transformer1Module1");
    QCOMPARE(modules[21]->m_module->getSCPIModuleName(), "TR11");

    QCOMPARE(modules[22]->m_moduleId, 1190);
    QCOMPARE(modules[22]->m_module->getVeinModuleName(), "AdjustmentModule1");
    QCOMPARE(modules[22]->m_module->getSCPIModuleName(), "ADJ1");

    QCOMPARE(modules[23]->m_moduleId, 1200);
    QCOMPARE(modules[23]->m_module->getVeinModuleName(), "SEM1Module1");
    QCOMPARE(modules[23]->m_module->getSCPIModuleName(), "EM01");

    QCOMPARE(modules[24]->m_moduleId, 1210);
    QCOMPARE(modules[24]->m_module->getVeinModuleName(), "SPM1Module1");
    QCOMPARE(modules[24]->m_module->getSCPIModuleName(), "PM01");

    QCOMPARE(modules[25]->m_moduleId, 1300);
    QCOMPARE(modules[25]->m_module->getVeinModuleName(), "SourceModule1");
    QCOMPARE(modules[25]->m_module->getSCPIModuleName(), "SRC1");

    QCOMPARE(modules[26]->m_moduleId, 1400);
    QCOMPARE(modules[26]->m_module->getVeinModuleName(), "BleModule1");
    QCOMPARE(modules[26]->m_module->getSCPIModuleName(), "BLE1");

    QCOMPARE(modules[27]->m_moduleId, 9999);
    QCOMPARE(modules[27]->m_module->getVeinModuleName(), "SCPIModule1");
    QCOMPARE(modules[27]->m_module->getSCPIModuleName(), "SCP1");

    modMan.destroyModulesAndWaitUntilAllShutdown();
    // ------ end auto generated code ----
}
