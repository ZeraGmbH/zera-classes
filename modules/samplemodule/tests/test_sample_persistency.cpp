#include "test_sample_persistency.h"
#include "modulemanagertestrunner.h"
#include "pcbserviceconnection.h"
#include "taskpllchannelget.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <QDir>
#include <QFile>
#include <QTest>

QTEST_MAIN(test_sample_persistency)

constexpr int sampleEntityId = 1030;

static const char* tmpPath = "/tmp/samplemodule";
static const char* tmpConf = "/tmp/samplemodule/module.xml";

void test_sample_persistency::init()
{
    TestLogHelpers::copyFile(":/module-configurations/mt310s2-samplemodule-UL1.xml", tmpConf);
}

void test_sample_persistency::cleanup()
{
    QDir dir(tmpPath);
    dir.removeRecursively();
}

void test_sample_persistency::loadUL1SetUL2Reload()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();

    std::unique_ptr<ModuleManagerTestRunner> testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/sample-for-persistency.json");
    std::unique_ptr<PcbServiceConnection> pcbConnection = std::make_unique<PcbServiceConnection>(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);
    QCOMPARE(testRunner->getVfComponent(sampleEntityId, "PAR_PllChannel"), "UL1");

    testRunner->setVfComponent(sampleEntityId, "PAR_PllChannel", "UL2");
    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    QCOMPARE(testRunner->getVfComponent(sampleEntityId, "PAR_PllChannel"), "UL2");
    TimeMachineObject::feedEventLoop();

    pcbConnection.reset();
    testRunner.reset();

    testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/sample-for-persistency.json");
    pcbConnection = std::make_unique<PcbServiceConnection>(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);
    QCOMPARE(testRunner->getVfComponent(sampleEntityId, "PAR_PllChannel"), "UL2");

    TaskContainerQueue taskQueue;
    taskQueue.addSub(pcbConnection->createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection->getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(*pllChannelMName, "m1");
}

void test_sample_persistency::loadPllAutoOffSetPllAutoOnReload()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();

    std::unique_ptr<ModuleManagerTestRunner> testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/sample-for-persistency.json");
    QCOMPARE(testRunner->getVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff"), 0);

    testRunner->setVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff", 1);
    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    QCOMPARE(testRunner->getVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff"), 1);
    TimeMachineObject::feedEventLoop();

    testRunner.reset();

    testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/sample-for-persistency.json");
    QCOMPARE(testRunner->getVfComponent(sampleEntityId, "PAR_PllAutomaticOnOff"), 1);
}
