#include "test_scpi_mem_leaks.h"
#include "modulemanagertestrunner.h"
#include "scpimodulenetclientblocked.h"
#include "scpiclient.h"
#include "scpiclientexecutormeasure.h"
#include <backtracetreegenerator.h>
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <memoryalloctracker.h>
#include <QTest>

QTEST_MAIN(test_scpi_mem_leaks)

void test_scpi_mem_leaks::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_scpi_mem_leaks::memLeakSorted()
{
    QSKIP("Mem test takes ages - just for analysis");

    MemoryAllocTracker memAllocTracker;
    memAllocTracker.start();

    {
        ModuleManagerTestRunner testRunner("mt310s2-meas-session.json");
        bool allOK = true;

        for (int i=0; i<10 && allOK; i++) {
            if (!runScpiIo(testRunner))
                allOK = false;
        }
        QVERIFY(allOK);
    }

    QTest::qWait(10000); // wait for QArrayData::allocate relax (QAbstractSocket)
    memAllocTracker.stop();
    BacktraceTreeGenerator allocTree(memAllocTracker.getAllocationsTimeSorted());
    qInfo("Unfreed %i", allocTree.getEntryList()->count());
}

void test_scpi_mem_leaks::checkLeftoverObjectsMeasureAndFriendsSorted()
{
    {
        ModuleManagerTestRunner testRunner("mt310s2-meas-session.json");
        testRunner.setVfComponent(9999, "PAR_QueryResponseSortActive", 1);

        bool allOK = true;
        for (int clientTransaction=0; clientTransaction<5 && allOK; clientTransaction++) {
            if (!runScpiIo(testRunner))
                allOK = false;
        }
        QVERIFY(allOK);
        QCOMPARE(SCPIMODULE::ScpiClientExecutorMeasure::getInstanceCount(), 0);
        QCOMPARE(SCPIMODULE::ScpiVeinComponentSequenceMeasure::getInstanceCount(), 0);
        QCOMPARE(SCPIMODULE::cSCPIClient::getInstanceCount(), 0);
        QCOMPARE(SCPIMODULE::ScpiVeinComponentSequenceMeasure::getInstanceCount(), 0);
    }
    QCOMPARE(SCPIMODULE::ScpiDelegateTemplate::getInstanceCount(), 0);
}

void test_scpi_mem_leaks::checkLeftoverObjectsMeasureAndFriendsUnsorted()
{
    {
        ModuleManagerTestRunner testRunner("mt310s2-meas-session.json");
        testRunner.setVfComponent(9999, "PAR_QueryResponseSortActive", 0);

        bool allOK = true;
        for (int clientTransaction=0; clientTransaction<5 && allOK; clientTransaction++) {
            if (!runScpiIo(testRunner))
                allOK = false;
        }
        QVERIFY(allOK);
        QCOMPARE(SCPIMODULE::ScpiClientExecutorMeasure::getInstanceCount(), 0);
        QCOMPARE(SCPIMODULE::ScpiVeinComponentSequenceMeasure::getInstanceCount(), 0);
        QCOMPARE(SCPIMODULE::cSCPIClient::getInstanceCount(), 0);
        QCOMPARE(SCPIMODULE::ScpiVeinComponentSequenceMeasure::getInstanceCount(), 0);
    }
    QCOMPARE(SCPIMODULE::ScpiDelegateTemplate::getInstanceCount(), 0);
}

bool test_scpi_mem_leaks::runScpiIo(ModuleManagerTestRunner &testRunner)
{
    bool allOK = true;
    {
        ScpiModuleNetClientBlocked client;

        client.sendMulti(QByteArrayList() << "INIT"); // no trailing semicolon???
        client.sendMulti(QByteArrayList() << "FETCH:DFT1?");
        client.sendMulti(QByteArrayList() << "FETCH:DFT1?");
        client.sendMulti(QByteArrayList() << "FETCH:DFT1:UL1?");
        client.sendMulti(QByteArrayList() << "MEAS:DFT1:UL1?");
        TimeMachineObject::feedEventLoop();
        testRunner.fireActualValues();
        TimeMachineObject::feedEventLoop();
        const QByteArrayList received = client.receiveMulti();
        if (received.count() != 4)
            allOK = false;
    }
    TimeMachineObject::feedEventLoop();
    return allOK;
}

