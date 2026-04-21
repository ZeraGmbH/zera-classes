#include "test_snapshot_recorder.h"
#include <contentsetsotherfromcontentsetsconfig.h>
#include <contentsetszeraallfrommodmansessions.h>
#include <loggercontentsetconfig.h>
#include <testsqlitedb.h>
#include <vl_sqlitedb.h>
#include <vf-cpp-entity.h>
#include <vf_client_rpc_invoker.h>
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <testloghelpers.h>
#include<QSignalSpy>
#include <QTest>

QTEST_MAIN(test_snapshot_recorder)

constexpr int dataLoggerEntityId = 2;
constexpr int recorderEntityId = 1800;
constexpr int rmsEntityId = 1040;
constexpr int sigMeasuringEntityId = 1050; //DftModule
constexpr int powerEntityId = 1070;
const QLatin1String DBPath= QLatin1String("/tmp/veindb-test/");

void test_snapshot_recorder::init()
{
    createModmanWithLogger();
    createModulesManually();
}

void test_snapshot_recorder::cleanup()
{
    m_testRunner = nullptr;
    QDir dirCustomer(DBPath);
    dirCustomer.removeRecursively();
}

void test_snapshot_recorder::takeSnapshotNoValuesCsvEmpty()
{
    QList<int> entityList = m_storageDb->getEntityList();
    QCOMPARE(entityList.count(), 30);
    QVERIFY(entityList.contains(dataLoggerEntityId));

    QString transactionName = "Snaphot";
    takeSnapshot("session", transactionName);

    m_rpcClient = std::make_shared<VfRPCInvoker>(dataLoggerEntityId, std::make_unique<VfClientRPCInvoker>());
    m_testRunner->getVfCmdEventHandlerSystemPtr()->addItem(m_rpcClient);
    QSignalSpy invokerSpy(m_rpcClient.get(), &VfRPCInvoker::sigRPCFinished);
    QVariantMap rpcParams;
    rpcParams.insert("p_transaction", transactionName);
    m_rpcClient->invokeRPC("RPC_displayActualValues", rpcParams);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(invokerSpy.count(), 1);

    QVariantMap argMap = invokerSpy[0][1].toMap();
    QVariant result = argMap.value(VeinComponent::RemoteProcedureData::s_resultCodeString);
    QCOMPARE(result, VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);

    QJsonObject jsonObj = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QJsonObject recorderObj = jsonObj[QString::number(recorderEntityId)].toObject();

    QString csvDumped = recorderObj["ACT_CSV_EXPORT"].toString();
    QString csvExpected = TestLogHelpers::loadFile(":/fireValuesNone.csv");

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(csvExpected, csvDumped));
}

void test_snapshot_recorder::takeSnapshotOneValueCsv()
{
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);

    TimeMachineForTest::getInstance()->setCurrentTime(QDate(2026, 2, 25),
                                                      QTime(19, 21, 37, 1),
                                                      QTimeZone(3*3600));
    fireActualValues();
    triggerDftModuleSigMeasuring();

    QString transactionName = "Snaphot";
    takeSnapshot("session", transactionName);

    m_rpcClient = std::make_shared<VfRPCInvoker>(dataLoggerEntityId, std::make_unique<VfClientRPCInvoker>());
    m_testRunner->getVfCmdEventHandlerSystemPtr()->addItem(m_rpcClient);
    QSignalSpy invokerSpy(m_rpcClient.get(), &VfRPCInvoker::sigRPCFinished);
    QVariantMap rpcParams;
    rpcParams.insert("p_transaction", transactionName);
    m_rpcClient->invokeRPC("RPC_displayActualValues", rpcParams);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(invokerSpy.count(), 1);

    QVariantMap argMap = invokerSpy[0][1].toMap();
    QVariant result = argMap.value(VeinComponent::RemoteProcedureData::s_resultCodeString);
    QCOMPARE(result, VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);

    QJsonObject jsonObj = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QJsonObject recorderObj = jsonObj[QString::number(recorderEntityId)].toObject();

    QString csvDumped = recorderObj["ACT_CSV_EXPORT"].toString();
    QString csvExpected = TestLogHelpers::loadFile(":/fireValuesOnce.csv");

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(csvExpected, csvDumped));
}

void test_snapshot_recorder::takeSnapshotMaximumValuesCsvFull()
{
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    TimeMachineForTest::getInstance()->setCurrentTime(QDate(2026, 2, 25),
                                                      QTime(19, 21, 37, 1),
                                                      QTimeZone(3*3600));

    constexpr int actualValueCount = 2400; //equivalent of one value per 500ms for 20mins
    constexpr int measPeriodMs = 10;
    for (int valueNo=0; valueNo<actualValueCount; valueNo++) {
        fireActualValues();
        triggerDftModuleSigMeasuring();
        TimeMachineForTest::getInstance()->processTimers(measPeriodMs);
    }

    QString transactionName = "Snaphot";
    takeSnapshot("session", transactionName);

    m_rpcClient = std::make_shared<VfRPCInvoker>(dataLoggerEntityId, std::make_unique<VfClientRPCInvoker>());
    m_testRunner->getVfCmdEventHandlerSystemPtr()->addItem(m_rpcClient);
    QSignalSpy invokerSpy(m_rpcClient.get(), &VfRPCInvoker::sigRPCFinished);
    QVariantMap rpcParams;
    rpcParams.insert("p_transaction", transactionName);
    m_rpcClient->invokeRPC("RPC_displayActualValues", rpcParams);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(invokerSpy.count(), 1);

    QVariantMap argMap = invokerSpy[0][1].toMap();
    QVariant result = argMap.value(VeinComponent::RemoteProcedureData::s_resultCodeString);
    QCOMPARE(result, VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);

    QJsonObject jsonObj = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QJsonObject recorderObj = jsonObj[QString::number(recorderEntityId)].toObject();

    QString csvDumped = recorderObj["ACT_CSV_EXPORT"].toString();
    QString csvExpected = TestLogHelpers::loadFile(":/fireMaxValues.csv");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(csvExpected, csvDumped));

    QStringList lines = csvExpected.split('\n', Qt::SkipEmptyParts);
    QCOMPARE(lines.size(), actualValueCount+1);
}

void test_snapshot_recorder::createModmanWithLogger()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("", true, "mt310s2",
                                                             MockLxdmSessionChangeParamGenerator::generateTestSessionChanger(),
                                                             true, true);
    m_storageDb = m_testRunner->getVeinStorageDb();

    ModuleManagerSetupFacade* mmFacade = m_testRunner->getModManFacade();

    QString sessionFileName = "mt310s2-emob-session-ac.json";
    m_testRunner->start(sessionFileName);
    mmFacade->getLicenseSystem()->setDeviceSerial("foo");
    TimeMachineObject::feedEventLoop();
    mmFacade->getSystemModuleEventSystem()->initializeEntity(sessionFileName, QStringList() << sessionFileName);
    TimeMachineObject::feedEventLoop();
}

void test_snapshot_recorder::createModulesManually()
{
    QVariantMap components = {
        {"ACT_RMSPN1", QVariant()},
        {"ACT_RMSPN2", QVariant()},
        {"ACT_RMSPN3", QVariant()},
        {"ACT_RMSPN4", QVariant()},
        {"ACT_RMSPN5", QVariant()},
        {"ACT_RMSPN6", QVariant()}
    };
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {
        {"ACT_PQS1", QVariant()},
        {"ACT_PQS2", QVariant()},
        {"ACT_PQS3", QVariant()},
        {"ACT_PQS4", QVariant()}
    };
    createModule(powerEntityId, components);
}

void test_snapshot_recorder::createModule(int entityId, QMap<QString, QVariant> components)
{
    VfCpp::VfCppEntity * entity =new VfCpp::VfCppEntity(entityId);
    m_testRunner->getModManFacade()->addSubsystem(entity);
    entity->initModule();
    for (auto it = components.constBegin(); it != components.constEnd(); ++it)
        entity->createComponent(it.key(), it.value());
    TimeMachineObject::feedEventLoop();
}

void test_snapshot_recorder::fireActualValues()
{
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN1", 1.1);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN2", 1.0+(1.0/1e6));
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN3", 3.333333e5);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN4", 4.45959595e5);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN5", 5.55555555595e-7);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN6", 6.67895678e-3);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS1", 1200.676767676);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS2", 13333337779.777888999);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS3", 0.0000000123456789);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS4", 0.100000565656);
}

void test_snapshot_recorder::triggerDftModuleSigMeasuring()
{
    //"SIG_Measuring" changes from 0 to 1 when new actual values are available
    m_testRunner->setVfComponent(sigMeasuringEntityId, "SIG_Measuring", QVariant(0));
    m_testRunner->setVfComponent(sigMeasuringEntityId, "SIG_Measuring", QVariant(1));
}

void test_snapshot_recorder::takeSnapshot(QString sessionName, QString transactionName)
{
    QString DBName = DBPath + "DB_NAME";
    m_testRunner->setVfComponent(dataLoggerEntityId, "DatabaseFile", DBName);
    m_testRunner->setVfComponent(dataLoggerEntityId, "sessionName", sessionName);
    m_testRunner->setVfComponent(dataLoggerEntityId, "guiContext", "ZeraGuiActualValues");
    m_testRunner->setVfComponent(dataLoggerEntityId, "currentContentSets", QStringList() << "ZeraComparison");
    m_testRunner->setVfComponent(dataLoggerEntityId, "transactionName", transactionName);
    //startLogging
    m_testRunner->setVfComponent(dataLoggerEntityId, "LoggingEnabled", true);
    //stopLogging
    m_testRunner->setVfComponent(dataLoggerEntityId, "LoggingEnabled", false);
}
