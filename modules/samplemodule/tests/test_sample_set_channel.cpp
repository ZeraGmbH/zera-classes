#include "test_sample_set_channel.h"
#include "modulemanagertestrunner.h"
#include "pcbserviceconnection.h"
#include "taskpllchannelget.h"
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_sample_set_channel)

static int constexpr sampleEntityId = 1030;

void test_sample_set_channel::startUL1NoFixNoAutoSetInvalid()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-UL1.json");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "UL1");

    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();
    PcbServiceConnection pcbConnection(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);

    testRunner.setVfComponent(sampleEntityId, "PAR_PllChannel", "foo");
    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "UL1");

    TaskContainerQueue taskQueue;
    taskQueue.addSub(pcbConnection.createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection.getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*pllChannelMName, "m0");
}

void test_sample_set_channel::startUL1NoFixNoAutoSetUL2()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-UL1.json");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "UL1");

    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();
    PcbServiceConnection pcbConnection(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);

    testRunner.setVfComponent(sampleEntityId, "PAR_PllChannel", "UL2");
    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "UL2");

    TaskContainerQueue taskQueue;
    taskQueue.addSub(pcbConnection.createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection.getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*pllChannelMName, "m1");
}

void test_sample_set_channel::startIL1NoFixAutoSetUL2NoChange()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-IL1-auto-on.json");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "IL1");

    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();
    PcbServiceConnection pcbConnection(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);

    testRunner.setVfComponent(sampleEntityId, "PAR_PllChannel", "UL2");
    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "IL1");

    TaskContainerQueue taskQueue;
    taskQueue.addSub(pcbConnection.createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection.getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*pllChannelMName, "m3");
}

void test_sample_set_channel::startFixAutoNotSetUL2NoChange()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-fixed-freq.json");
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "");

    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();
    PcbServiceConnection pcbConnection(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);

    testRunner.setVfComponent(sampleEntityId, "PAR_PllChannel", "UL2");
    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    QCOMPARE(testRunner.getVfComponent(sampleEntityId, "PAR_PllChannel"), "");

    TaskContainerQueue taskQueue;
    taskQueue.addSub(pcbConnection.createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection.getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*pllChannelMName, "13Hz_DPLL");
}
