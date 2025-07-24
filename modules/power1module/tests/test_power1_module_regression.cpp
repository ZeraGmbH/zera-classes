#include "test_power1_module_regression.h"
#include "modulemanagertestrunner.h"
#include "modulemanagerconfig.h"
#include "power1modulemeasprogram.h"
#include "scpimoduleclientblocked.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <vs_dumpjson.h>
#include <vf_entity_component_event_item.h>
#include <vf_client_component_setter.h>
#include <timemachineobject.h>
#include <xmldocumentcompare.h>
#include <testloghelpers.h>
#include <QJsonArray>
#include <QTest>

QTEST_MAIN(test_power1_module_regression)

const int powerEntityId = 1070;

void test_power1_module_regression::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_power1_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/sessions/session-minimal.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    VeinStorage::AbstractDatabase* storageDb = veinStorage->getDb();

    QList<int> entityList = storageDb->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageDb->hasEntity(powerEntityId));
}

void test_power1_module_regression::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/session-power1-test.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    VeinStorage::AbstractDatabase* storageDb = veinStorage->getDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(storageDb, QList<int>() << powerEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_power1_module_regression::injectActualValues()
{
    ModuleManagerTestRunner testRunner(":/sessions/session-power1-test.json");
    TestDspInterfacePtr power1DspInterface = testRunner.getDspInterface(powerEntityId);

    QVector<float> powerValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powerValues.append(i);
    powerValues.append(powerValues[0] + powerValues[1] + powerValues[2]);

    power1DspInterface->fireActValInterrupt(powerValues, irqNr);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpActual.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << powerEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_power1_module_regression::testScpiCommandsDisabled()
{
    ModuleManagerTestRunner testRunner(":/sessions/session-power1-withoutScpi-test.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial-withoutScpi.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    VeinStorage::AbstractDatabase* storageDb = veinStorage->getDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(storageDb, QList<int>() << powerEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));

    //When we change into 'INF_ModuleInterface', we take current 'jsonDumped' and copy it to test-data.
    //So, here is an extra check to be sure that no scpi commands are introduced by accident in future.
    QJsonObject dumpedModuleInterface = QJsonDocument::fromVariant(storageDb->getStoredValue(powerEntityId, QString("INF_ModuleInterface"))).object();
    QJsonValue scpiCmds = dumpedModuleInterface.value("SCPIInfo").toObject().value("Cmd");
    QCOMPARE(scpiCmds.toArray().count(), 0);
}

void test_power1_module_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/sessions/session-minimal.json");
    TestDspInterfacePtr power1DspInterface = testRunner.getDspInterface(powerEntityId);

    QString measProgramDumped = TestLogHelpers::dump(power1DspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}

const int powerEntityIdP1 = 1070;
const int powerEntityIdP2 = 1071;
const int powerEntityIdP3 = 1072;
const int powerEntityIdP4 = 1073;

void test_power1_module_regression::dumpDspSetupMt310s2Pow1To4()
{
    ModuleManagerTestRunner testRunner(":/sessions/session-mt310s2-p1-p4.json", "mt310s2");

    TestDspInterfacePtr power1DspInterfaceP1 = testRunner.getDspInterface(powerEntityIdP1);
    QString measProgramDumpedP1 = TestLogHelpers::dump(power1DspInterfaceP1->dumpAll());
    QString measProgramExpectedP1 = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgramMt310s2P1.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpectedP1, measProgramDumpedP1));

    TestDspInterfacePtr power1DspInterfaceP2 = testRunner.getDspInterface(powerEntityIdP2);
    QString measProgramDumpedP2 = TestLogHelpers::dump(power1DspInterfaceP2->dumpAll());
    QString measProgramExpectedP2 = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgramMt310s2P2.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpectedP2, measProgramDumpedP2));

    TestDspInterfacePtr power1DspInterfaceP3 = testRunner.getDspInterface(powerEntityIdP3);
    QString measProgramDumpedP3 = TestLogHelpers::dump(power1DspInterfaceP3->dumpAll());
    QString measProgramExpectedP3 = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgramMt310s2P3.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpectedP3, measProgramDumpedP3));

    TestDspInterfacePtr power1DspInterfaceP4 = testRunner.getDspInterface(powerEntityIdP4);
    QString measProgramDumpedP4 = TestLogHelpers::dump(power1DspInterfaceP4->dumpAll());
    QString measProgramExpectedP4 = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgramMt310s2P4.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpectedP4, measProgramDumpedP4));
}

void test_power1_module_regression::dumpDspSetupCom5003Pow1To4()
{
    ModuleManagerTestRunner testRunner(":/sessions/session-com5003-p1-p4.json", "com5003");

    TestDspInterfacePtr power1DspInterfaceP1 = testRunner.getDspInterface(powerEntityIdP1);
    QString measProgramDumpedP1 = TestLogHelpers::dump(power1DspInterfaceP1->dumpAll());
    QString measProgramExpectedP1 = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgramCom5003P1.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpectedP1, measProgramDumpedP1));

    TestDspInterfacePtr power1DspInterfaceP2 = testRunner.getDspInterface(powerEntityIdP2);
    QString measProgramDumpedP2 = TestLogHelpers::dump(power1DspInterfaceP2->dumpAll());
    QString measProgramExpectedP2 = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgramCom5003P2.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpectedP2, measProgramDumpedP2));

    TestDspInterfacePtr power1DspInterfaceP3 = testRunner.getDspInterface(powerEntityIdP3);
    QString measProgramDumpedP3 = TestLogHelpers::dump(power1DspInterfaceP3->dumpAll());
    QString measProgramExpectedP3 = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgramCom5003P3.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpectedP3, measProgramDumpedP3));

    TestDspInterfacePtr power1DspInterfaceP4 = testRunner.getDspInterface(powerEntityIdP4);
    QString measProgramDumpedP4 = TestLogHelpers::dump(power1DspInterfaceP4->dumpAll());
    QString measProgramExpectedP4 = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgramCom5003P4.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpectedP4, measProgramDumpedP4));
}

void test_power1_module_regression::dumpDspOnMeasModeChange()
{
    ModuleManagerTestRunner testRunner(":/sessions/session-minimal.json");
    TestDspInterfacePtr power1DspInterface = testRunner.getDspInterface(powerEntityId);

    setMeasMode(powerEntityId, testRunner.getVfCmdEventHandlerSystemPtr(), "4LW", "3LW");
    setMeasMode(powerEntityId, testRunner.getVfCmdEventHandlerSystemPtr(), "3LW", "2LW");
    setMeasMode(powerEntityId, testRunner.getVfCmdEventHandlerSystemPtr(), "2LW", "4LW");

    QString measProgramDumped = TestLogHelpers::dump(power1DspInterface->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-measmode-change.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}

void test_power1_module_regression::dumpDspOnMeasModeChangeApparentGeom()
{
    ModuleManagerTestRunner testRunner(":/sessions/session-com5003-p1-p4.json", "com5003");
    TestDspInterfacePtr power1DspInterface = testRunner.getDspInterface(powerEntityIdP4);

    setMeasMode(powerEntityIdP4, testRunner.getVfCmdEventHandlerSystemPtr(), "4LW", "4LS");
    setMeasMode(powerEntityIdP4, testRunner.getVfCmdEventHandlerSystemPtr(), "4LS", "4LSg");
    setMeasMode(powerEntityIdP4, testRunner.getVfCmdEventHandlerSystemPtr(), "4LSg", "4LW");

    setMeasMode(powerEntityIdP4, testRunner.getVfCmdEventHandlerSystemPtr(), "4LW", "3LS");
    setMeasMode(powerEntityIdP4, testRunner.getVfCmdEventHandlerSystemPtr(), "3LS", "3LSg");
    setMeasMode(powerEntityIdP4, testRunner.getVfCmdEventHandlerSystemPtr(), "3LSg", "4LW");

    setMeasMode(powerEntityIdP4, testRunner.getVfCmdEventHandlerSystemPtr(), "4LW", "2LS");
    setMeasMode(powerEntityIdP4, testRunner.getVfCmdEventHandlerSystemPtr(), "2LS", "2LSg");
    setMeasMode(powerEntityIdP4, testRunner.getVfCmdEventHandlerSystemPtr(), "2LSg", "4LW");

    QString measProgramDumped = TestLogHelpers::dump(power1DspInterface->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-measmode-change-apparent-geometric.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}

void test_power1_module_regression::scpiDumpMtPower1Module1()
{
    ModulemanagerConfig::setDemoDevice("mt310s2");
    ModuleManagerTestRunner testRunner(":/sessions/session-power1module1-scpi.json");

    ScpiModuleClientBlocked client;
    QString dumped = client.sendReceive("dev:iface?", false);

    QFile ifaceBaseXmlFile("://scpiDumps/session-power1module1-scpi.xml");
    QVERIFY(ifaceBaseXmlFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    QString expected = ifaceBaseXmlFile.readAll();

    XmlDocumentCompare compare;
    bool ok = compare.compareXml(dumped, expected);
    if(!ok)
        TestLogHelpers::compareAndLogOnDiff(expected, dumped);
    QVERIFY(ok);
}

void test_power1_module_regression::scpiDumpMtPower1Module4()
{
    ModulemanagerConfig::setDemoDevice("mt310s2");
    ModuleManagerTestRunner testRunner(":/sessions/session-power1module4-scpi.json");

    ScpiModuleClientBlocked client;
    QString dumped = client.sendReceive("dev:iface?", false);

    QFile ifaceBaseXmlFile("://scpiDumps/session-power1module4-scpi.xml");
    QVERIFY(ifaceBaseXmlFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    QString expected = ifaceBaseXmlFile.readAll();

    XmlDocumentCompare compare;
    bool ok = compare.compareXml(dumped, expected);
    if(!ok)
        TestLogHelpers::compareAndLogOnDiff(expected, dumped);
    QVERIFY(ok);
}

void test_power1_module_regression::setMeasMode(int entityId,
                                                VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem,
                                                QString measModeOld,
                                                QString measModeNew)
{
    VfCmdEventItemEntityPtr entityItem = VfEntityComponentEventItem::create(entityId);
    vfCmdEventHandlerSystem->addItem(entityItem);

    VfClientComponentSetterPtr setter = VfClientComponentSetter::create("PAR_MeasuringMode", entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent(measModeOld, measModeNew);
    TimeMachineObject::feedEventLoop();
}

