#include "test_dspsuper_module.h"
#include "dspsupermodule.h"
#include "modulemanagertestrunner.h"
#include <zdspdumpfunctions.h>
#include "dspsupertestsupport.h"
#include <timemachineobject.h>
#include <vf_core_stack_client.h>
#include <vf_entity_component_event_item.h>
#include <vf_client_component_setter.h>
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_dspsuper_module)

static int constexpr dspSuperEntityId = 9000;

void test_dspsuper_module::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_dspsuper_module::init()
{
    TimeMachineForTest::reset();
    TimeMachineForTest::getInstance()->setCurrentTime(QDate(2026, 3, 21),
                                                      QTime(23, 59, 37, 1),
                                                      QTimeZone(3600));
}

void test_dspsuper_module::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QList<int> entityList = veinStorageDb->getEntityList();

    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorageDb->hasEntity(dspSuperEntityId));
}

void test_dspsuper_module::dspDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(dspSuperEntityId);
    QString measProgramDumped = TestLogHelpers::dump(dspInterface->dumpAll());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/dspSourceDumps/dumpInitial.json", measProgramDumped));
}

void test_dspsuper_module::zdspDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    ZDspServer* server = testRunner.getDspServer();
    QString dumped = TestLogHelpers::dump(ZDspDumpFunctions::getFullDump(server));
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/zdspDumps/dumpInitial.json", dumped));
}

void test_dspsuper_module::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray dumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << dspSuperEntityId);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/veinDumps/dumpInitial.json", dumped));
}

void test_dspsuper_module::supervisorMapInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    DSPSUPERMODULE::DspSuperModule *dspSuperModule = qobject_cast<DSPSUPERMODULE::DspSuperModule*>(testRunner.getModule("dspsupermodule", 9000));
    DspSupervisorOutputMap map = dspSuperModule->getDspCommonSupervisor()->getSupervisorMap();
    QCOMPARE(map.size(), 0);
}

void test_dspsuper_module::supervisorMapOneInterrupt()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(dspSuperEntityId);
    DSPSUPERMODULE::DspSuperModule *dspSuperModule = qobject_cast<DSPSUPERMODULE::DspSuperModule*>(testRunner.getModule("dspsupermodule", 9000));

    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentPeriod(), 0);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentMsTime(), 0);
    DspSuperTestSupport::fireInterrupt({12.5, 1, 20}, dspInterface);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentPeriod(), 1);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentMsTime(), 20);

    DspSupervisorOutputMap map = dspSuperModule->getDspCommonSupervisor()->getSupervisorMap();
    QVERIFY(map.contains(1));
    QCOMPARE(map[1].m_rawIn.m_periodCount, 1);
    QCOMPARE(map[1].m_rawIn.m_msTimer, 20);
    QCOMPARE(map[1].m_rawIn.m_percentBusy, 12.5);
    QCOMPARE(map[1].m_timeStamp, TimerFactoryQt::getCurrentTime());
}

void test_dspsuper_module::supervisorMapOneInterruptTwoEntries()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(dspSuperEntityId);
    DSPSUPERMODULE::DspSuperModule *dspSuperModule = qobject_cast<DSPSUPERMODULE::DspSuperModule*>(testRunner.getModule("dspsupermodule", 9000));

    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentPeriod(), 0);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentMsTime(), 0);
    DspSuperTestSupport::fireInterrupts({{12.5, 1, 20}, {11.5, 2, 40}}, dspInterface);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentPeriod(), 2);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentMsTime(), 40);

    DspSupervisorOutputMap map = dspSuperModule->getDspCommonSupervisor()->getSupervisorMap();
    QDateTime now = TimerFactoryQt::getCurrentTime();

    QCOMPARE(map.count(), 2);

    QVERIFY(map.contains(1));
    QCOMPARE(map[1].m_rawIn.m_periodCount, 1);
    QCOMPARE(map[1].m_rawIn.m_msTimer, 20);
    QCOMPARE(map[1].m_rawIn.m_percentBusy, 12.5);
    QCOMPARE(map[1].m_timeStamp, now.addMSecs(-qint64(20)));

    QVERIFY(map.contains(2));
    QCOMPARE(map[2].m_rawIn.m_periodCount, 2);
    QCOMPARE(map[2].m_rawIn.m_msTimer, 40);
    QCOMPARE(map[2].m_rawIn.m_percentBusy, 11.5);
    QCOMPARE(map[2].m_timeStamp, now);
}

void test_dspsuper_module::supervisorMapTwoInterrupts()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(dspSuperEntityId);
    DSPSUPERMODULE::DspSuperModule *dspSuperModule = qobject_cast<DSPSUPERMODULE::DspSuperModule*>(testRunner.getModule("dspsupermodule", 9000));
    QDateTime now = TimerFactoryQt::getCurrentTime();

    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentPeriod(), 0);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentMsTime(), 0);
    DspSuperTestSupport::fireInterrupt({12.5, 1, 20}, dspInterface);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentPeriod(), 1);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentMsTime(), 20);

    TimeMachineForTest::getInstance()->processTimers(20);
    DspSuperTestSupport::fireInterrupt({11.5, 2, 40}, dspInterface);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentPeriod(), 2);
    QCOMPARE(dspSuperModule->getDspCommonSupervisor()->getCurrentMsTime(), 40);

    DspSupervisorOutputMap map = dspSuperModule->getDspCommonSupervisor()->getSupervisorMap();
    QCOMPARE(map.count(), 2);

    QVERIFY(map.contains(1));
    QCOMPARE(map[1].m_rawIn.m_periodCount, 1);
    QCOMPARE(map[1].m_rawIn.m_msTimer, 20);
    QCOMPARE(map[1].m_rawIn.m_percentBusy, 12.5);
    QCOMPARE(map[1].m_timeStamp, now);

    QVERIFY(map.contains(2));
    QCOMPARE(map[2].m_rawIn.m_periodCount, 2);
    QCOMPARE(map[2].m_rawIn.m_msTimer, 40);
    QCOMPARE(map[2].m_rawIn.m_percentBusy, 11.5);
    QCOMPARE(map[2].m_timeStamp, now.addMSecs(20));
}

void test_dspsuper_module::supervisorMapThreeInterruptsTimerb32BitTurnaround()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(dspSuperEntityId);

    quint32 timer = std::numeric_limits<quint32>::max();
    quint32 period = 1;
    QDateTime now = TimerFactoryQt::getCurrentTime();
    DspSuperTestSupport::fireInterrupt({12.5, period++, timer++}, dspInterface);
    TimeMachineForTest::getInstance()->processTimers(1);
    DspSuperTestSupport::fireInterrupt({11.5, period++, timer++}, dspInterface);
    TimeMachineForTest::getInstance()->processTimers(1);
    DspSuperTestSupport::fireInterrupt({13.5, period++, timer++}, dspInterface);

    DSPSUPERMODULE::DspSuperModule *dspSuperModule = qobject_cast<DSPSUPERMODULE::DspSuperModule*>(testRunner.getModule("dspsupermodule", 9000));
    DspSupervisorOutputMap map = dspSuperModule->getDspCommonSupervisor()->getSupervisorMap();

    QCOMPARE(map.count(), 3);

    QVERIFY(map.contains(1));
    QCOMPARE(map[1].m_rawIn.m_periodCount, 1);
    QCOMPARE(map[1].m_rawIn.m_msTimer, std::numeric_limits<quint32>::max());
    QCOMPARE(map[1].m_rawIn.m_percentBusy, 12.5);
    QCOMPARE(map[1].m_timeStamp, now);

    QVERIFY(map.contains(2));
    QCOMPARE(map[2].m_rawIn.m_periodCount, 2);
    QCOMPARE(map[2].m_rawIn.m_msTimer, 0);
    QCOMPARE(map[2].m_rawIn.m_percentBusy, 11.5);
    QCOMPARE(map[2].m_timeStamp, now.addMSecs(1));

    QVERIFY(map.contains(3));
    QCOMPARE(map[3].m_rawIn.m_periodCount, 3);
    QCOMPARE(map[3].m_rawIn.m_msTimer, 1);
    QCOMPARE(map[3].m_rawIn.m_percentBusy, 13.5);
    QCOMPARE(map[3].m_timeStamp, now.addMSecs(2));
}

void test_dspsuper_module::supervisorMap10EntriesTotal()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal-10-total.json");
    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(dspSuperEntityId);

    constexpr quint32 periodMaxInjected = 20;
    for (quint32 period=1; period<=periodMaxInjected; period++)
        DspSuperTestSupport::fireInterrupt({37.2, period, 20*period}, dspInterface);

    DSPSUPERMODULE::DspSuperModule *dspSuperModule = qobject_cast<DSPSUPERMODULE::DspSuperModule*>(testRunner.getModule("dspsupermodule", 9000));
    DspSupervisorOutputMap map = dspSuperModule->getDspCommonSupervisor()->getSupervisorMap();
    QCOMPARE(map.count(), 10);
    QVERIFY(map.contains(11));
    QVERIFY(map.contains(20));
}
