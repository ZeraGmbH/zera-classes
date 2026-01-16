#include "test_sample_config_load.h"
#include "modulemanagertestrunner.h"
#include "pcbserviceconnection.h"
#include "samplemoduleconfiguration.h"
#include "taskpllchannelget.h"
#include <vs_dumpjson.h>
#include <mocktcpnetworkfactory.h>
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_sample_config_load)

void test_sample_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_SAMPLEMODULE) + "/" + "com5003-samplemodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_sample_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SAMPLEMODULE)).entryInfoList(QStringList() << "*.xml");
    QVERIFY(!fileList.isEmpty());
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        SAMPLEMODULE::cSampleModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}

void test_sample_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SAMPLEMODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = TestLogHelpers::loadFile(fileInfo.absoluteFilePath());
        SAMPLEMODULE::cSampleModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        SAMPLEMODULE::cSampleModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}

void test_sample_config_load::noWritesOnModuleLoad()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-UL1.json");
    QCOMPARE(testRunner.getModuleConfigWriteCounts(), 0);
}

constexpr int sampleEntityd = 1030;

void test_sample_config_load::startChannelUL1AutomaticOff()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-UL1.json");

    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();
    PcbServiceConnection pcbConnection(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);

    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    TaskContainerQueue taskQueue;
    taskQueue.addSub(pcbConnection.createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection.getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(*pllChannelMName, "m0");

    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/automaticOffUL1.json");
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(),
                                                                   QList<int>() << sampleEntityd,
                                                                   QList<int>(),
                                                                   true);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_sample_config_load::startChannelUL2AutomaticOff()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-UL2.json");

    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();
    PcbServiceConnection pcbConnection(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);

    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    TaskContainerQueue taskQueue;
    taskQueue.addSub(pcbConnection.createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection.getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*pllChannelMName, "m1");

    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/automaticOffUL2.json");
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(),
                                                                   QList<int>() << sampleEntityd,
                                                                   QList<int>(),
                                                                   true);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_sample_config_load::startChannelFixedFreqAutomaticOff()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-fixed-freq.json");

    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();
    PcbServiceConnection pcbConnection(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);

    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    TaskContainerQueue taskQueue;
    taskQueue.addSub(pcbConnection.createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection.getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*pllChannelMName, "13Hz_DPLL");

    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/automaticOffFixedFreq.json");
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(),
                                                                   QList<int>() << sampleEntityd,
                                                                   QList<int>(),
                                                                   true);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_sample_config_load::startChannelIL1AutomaticOn()
{
    ModuleManagerTestRunner testRunner(":/sessions/sample-only-IL1-auto-on.json");

    VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory = VeinTcp::MockTcpNetworkFactory::create();
    PcbServiceConnection pcbConnection(NetworkConnectionInfo("127.0.0.1", 6307), networkFactory);

    std::shared_ptr<QString> pllChannelMName = std::make_unique<QString>();
    TaskContainerQueue taskQueue;
    taskQueue.addSub(pcbConnection.createConnectionTask());
    taskQueue.addSub(TaskPllChannelGet::create(pcbConnection.getInterface(),
                                               pllChannelMName,
                                               TRANSACTION_TIMEOUT));
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*pllChannelMName, "m3");

    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/automaticOnIL1.json");
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(),
                                                                   QList<int>() << sampleEntityd,
                                                                   QList<int>(),
                                                                   true);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}
