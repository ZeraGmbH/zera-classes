#include "test_readinstrumenttype.h"
#include "controllerpersitentdata.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_readInstrumentType)

static int constexpr serverPort = 4711;
static constexpr int systemEntityId = 0;
static constexpr int statusEntityId = 1150;

void test_readInstrumentType::init()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/status-min-session.json");
    m_netSystem = std::make_unique<VeinNet::NetworkSystem>();
    m_netSystem->setOperationMode(VeinNet::NetworkSystem::VNOM_SUBSCRIPTION);
    m_tcpSystem = std::make_unique<VeinNet::TcpSystem>(VeinTcp::MockTcpNetworkFactory::create());
    ModuleManagerSetupFacade* modManFacade = m_testRunner->getModManFacade();
    modManFacade->addSubsystem(m_netSystem.get());
    modManFacade->addSubsystem(m_tcpSystem.get());
    m_tcpSystem->startServer(serverPort);

    m_veinClientStack = std::make_unique<VfCoreStackClient>(VeinTcp::MockTcpNetworkFactory::create());
    m_veinClientStack->connectToServer("127.0.0.1", serverPort);
    m_veinClientStack->subscribeEntity(systemEntityId);
    m_veinClientStack->subscribeEntity(statusEntityId);
    TimeMachineObject::feedEventLoop();
    m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
}

void test_readInstrumentType::cleanup()
{
    m_testRunner.reset();
    m_scpiClient.reset();
    m_veinClientStack.reset();
    m_netSystem.reset();
    m_tcpSystem.reset();

    ControllerPersitentData::cleanupPersitentData();
}

void test_readInstrumentType::noControllerReadInstrumentType()
{
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(statusEntityId));

    QVariant value = m_testRunner->getVfComponent(statusEntityId, "INF_HotplugChannels");
    QCOMPARE(value.toString(), "IL1,IL2,IL3,IAUX");

    value = m_testRunner->getVfComponent(statusEntityId, "INF_Instrument");
    QVERIFY(value.toJsonObject().isEmpty());
}

void test_readInstrumentType::oneEmobReadInstrumentType()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    m_testRunner->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant value = m_testRunner->getVfComponent(statusEntityId, "INF_HotplugChannels");
    QCOMPARE(value.toString(), "IL1,IL2,IL3,IAUX");

    QString instrumentValue = m_testRunner->getVfComponent(statusEntityId, "INF_Instrument").toString();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(instrumentValue.toUtf8());
    QJsonObject jsonObject;
    if (jsonDoc.isObject())
        jsonObject = jsonDoc.object();
    QStringList keys = jsonObject.keys();
    QCOMPARE(keys.size(), 1);
    QVERIFY(keys.at(0)=="IL1");
    QCOMPARE(jsonObject[keys.at(0)], "EMOB");
}

void test_readInstrumentType::oneMt650ReadInstrumentType()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"Mt", cClamp::undefined});
    m_testRunner->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QString instrumentValue = m_testRunner->getVfComponent(statusEntityId, "INF_Instrument").toString();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(instrumentValue.toUtf8());
    QJsonObject jsonObject;
    if (jsonDoc.isObject())
        jsonObject = jsonDoc.object();
    QStringList keys = jsonObject.keys();
    QCOMPARE(keys.size(), 1);
    QVERIFY(keys.at(0)=="IL1");
    QCOMPARE(jsonObject[keys.at(0)], "MT650e");
}

void test_readInstrumentType::oneEmobOneMt650ReadInstrumentType()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"Mt", cClamp::undefined});
    infoMap.insert("IAUX", {"Emob", cClamp::undefined});
    m_testRunner->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QString instrumentValue = m_testRunner->getVfComponent(statusEntityId, "INF_Instrument").toString();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(instrumentValue.toUtf8());
    QJsonObject jsonObject;
    if (jsonDoc.isObject())
        jsonObject = jsonDoc.object();
    QStringList keys = jsonObject.keys();
    QCOMPARE(keys.size(), 2);
    QVERIFY(keys.contains("IL1"));
    QCOMPARE(jsonObject["IL1"], "MT650e");
    QVERIFY(keys.contains("IAUX"));
    QCOMPARE(jsonObject["IAUX"], "EMOB");
}


void test_readInstrumentType::TwoEmobsReadInstrumentType()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"Emob1", cClamp::undefined});
    infoMap.insert("IAUX", {"Emob2", cClamp::undefined});
    m_testRunner->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QString instrumentValue = m_testRunner->getVfComponent(statusEntityId, "INF_Instrument").toString();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(instrumentValue.toUtf8());
    QJsonObject jsonObject;
    if (jsonDoc.isObject())
        jsonObject = jsonDoc.object();
    QStringList keys = jsonObject.keys();
    QCOMPARE(keys.size(), 2);
    QVERIFY(keys.contains("IL1"));
    QCOMPARE(jsonObject["IL1"], "EMOB");
    QVERIFY(keys.contains("IAUX"));
    QCOMPARE(jsonObject["IAUX"], "EMOB");
}

void test_readInstrumentType::TwoMt650ReadInstrumentType()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"Mt650e1", cClamp::undefined});
    infoMap.insert("IAUX", {"Mt650e2", cClamp::undefined});
    m_testRunner->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QVariant value = m_testRunner->getVfComponent(statusEntityId, "INF_HotplugChannels");
    QCOMPARE(value.toString(), "IL1,IL2,IL3,IAUX");
    QString instrumentValue = m_testRunner->getVfComponent(statusEntityId, "INF_Instrument").toString();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(instrumentValue.toUtf8());
    QJsonObject jsonObject;
    if (jsonDoc.isObject())
        jsonObject = jsonDoc.object();
    QStringList keys = jsonObject.keys();
    QCOMPARE(keys.size(), 2);
    QVERIFY(keys.contains("IL1"));
    QCOMPARE(jsonObject["IL1"], "MT650e");
    QVERIFY(keys.contains("IAUX"));
    QCOMPARE(jsonObject["IAUX"], "MT650e");
}

void test_readInstrumentType::readHotplugChannelsCom5003()
{
    //switch to com device
    m_testRunner.reset();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/com5003-min-session.json", false, "com5003");
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(statusEntityId));

    QVariant value = m_testRunner->getVfComponent(statusEntityId, "INF_HotplugChannels");
    QCOMPARE(value.toString(), "");
}
