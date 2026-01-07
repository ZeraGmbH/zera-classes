#include "test_recorder.h"
#include "veindatacollector.h"
//#include "recordermoduleinit.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <vf-cpp-entity.h>
#include <vf_client_rpc_invoker.h>
#include <vf_rpc_invoker.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_recorder)

constexpr int rmsEntityId = 1040;
constexpr int powerEntityId = 1070;
constexpr int recorderEntityId = 1800;

void test_recorder::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_recorder::init()
{
    m_testRunner= std::make_unique<ModuleManagerTestRunner>(":/mt310s2-emob-session-ac.json");
}

void test_recorder::cleanup()
{
    m_testRunner = nullptr;
}

void test_recorder::startLoggingOneRecording()
{
    QList<int> entityList = m_testRunner->getVeinStorageSystem()->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);

    //create modules manually to be able to set actual values
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);

    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);

    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);

    entityList = m_testRunner->getVeinStorageSystem()->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 5);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);

    QVariant num = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "ACT_Points");
    QCOMPARE(num, 1);
}

void test_recorder::startLoggingMultipleRecordings()
{
    //create modules manually to be able to set actual values
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);

    QVariant num = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "ACT_Points");
    QCOMPARE(num, 3);
}

void test_recorder::startStopRecordingTimerExpired()
{
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();

    TimeMachineForTest::getInstance()->processTimers(1199999);
    QVariant startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, true);
    TimeMachineForTest::getInstance()->processTimers(1);
    startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, false);
}

void test_recorder::startStopTwoRecordingsNoTimerExpired()
{
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(1199999); //19min59s
    QVariant startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, true);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", false);
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(1199999); //19min59s
    startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, true);
}

void test_recorder::startStopTwoRecordingsFirstRecordingTimerExpired()
{
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(1200000); //20mins
    QVariant startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, false);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(1199999); //19min59s
    startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, true);
}

void test_recorder::startStopTwoRecordingsSecondRecordingTimerExpired()
{
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(1199999); //19min59s
    QVariant startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, true);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", false);
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(1200000); //20mins
    startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, false);
}

void test_recorder::startStopTwoRecordingsBothRecordingTimersExpired()
{
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(1200000); //20min
    QVariant startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, false);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(1200000); //20min
    startStopLogging = m_testRunner->getVeinStorageSystem()->getDb()->getStoredValue(recorderEntityId, "PAR_StartStopRecording");
    QCOMPARE(startStopLogging, false);
}

void test_recorder::invokeRpcNoValuesRecorded()
{
    VfRPCInvokerPtr rpc = std::make_shared<VfRPCInvoker>(recorderEntityId, std::make_unique<VfClientRPCInvoker>());
    m_testRunner->getVfCmdEventHandlerSystemPtr()->addItem(rpc);

    QSignalSpy spy(rpc.get(), &VfRPCInvoker::sigRPCFinished);
    QVariantMap rpcParams;
    rpcParams.insert("p_endingPoint", 1);
    rpcParams.insert("p_startingPoint", 0);
    rpc->invokeRPC("RPC_ReadRecordedValues", rpcParams);
    TimeMachineObject::feedEventLoop();

    QVERIFY(spy.count() == 1);
    QVariantMap argMap = spy[0][2].toMap();
    QVariant errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(!errorMsg.toString().isEmpty());
    QVariant result = argMap[VeinComponent::RemoteProcedureData::s_returnString];
    QVERIFY(result.toString().isEmpty());
}

void test_recorder::invokeRpcOneValueRecorded()
{
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);

    VfRPCInvokerPtr rpc = std::make_shared<VfRPCInvoker>(recorderEntityId, std::make_unique<VfClientRPCInvoker>());
    m_testRunner->getVfCmdEventHandlerSystemPtr()->addItem(rpc);

    QSignalSpy spy(rpc.get(), &VfRPCInvoker::sigRPCFinished);
    QVariantMap rpcParams;
    rpcParams.insert("p_endingPoint", 1);
    rpcParams.insert("p_startingPoint", 0);
    rpc->invokeRPC("RPC_ReadRecordedValues", rpcParams);
    TimeMachineObject::feedEventLoop();

    QVERIFY(spy.count() == 1);
    QVariantMap argMap = spy[0][2].toMap();
    QVariant errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(errorMsg.toString().isEmpty());

    QJsonObject result = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QVERIFY(result.size()==1);
    QJsonValue value = result.value(result.keys().at(0)); //first timestamp
    QStringList entities = value.toObject().keys();
    QVERIFY(entities.size() == 2);
    QVERIFY(entities.contains(QString::number(powerEntityId)));
    QVERIFY(entities.contains(QString::number(rmsEntityId)));
}

void test_recorder::invokeRpcMultipleValuesRecorded()
{
    //create modules manually to be able to set actual values
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);

    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);

    VfRPCInvokerPtr rpc = std::make_shared<VfRPCInvoker>(recorderEntityId, std::make_unique<VfClientRPCInvoker>());
    m_testRunner->getVfCmdEventHandlerSystemPtr()->addItem(rpc);

    QSignalSpy spy(rpc.get(), &VfRPCInvoker::sigRPCFinished);
    QVariantMap rpcParams;
    rpcParams.insert("p_endingPoint", 4);
    rpcParams.insert("p_startingPoint", 2);
    rpc->invokeRPC("RPC_ReadRecordedValues", rpcParams);
    TimeMachineObject::feedEventLoop();

    QVERIFY(spy.count() == 1);
    QVariantMap argMap = spy[0][2].toMap();
    QVariant errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(errorMsg.toString().isEmpty());

    QJsonObject result = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QVERIFY(result.size() == 2);
    QJsonValue value = result.value(result.keys().at(0)); //first timestamp
    QStringList entities = value.toObject().keys();
    QVERIFY(entities.size() == 2);
    QVERIFY(entities.contains(QString::number(powerEntityId)));
    QVERIFY(entities.contains(QString::number(rmsEntityId)));

    value = result.value(result.keys().at(1)); //second timestamp
    entities = value.toObject().keys();
    QVERIFY(entities.size() == 2);
    QVERIFY(entities.contains(QString::number(powerEntityId)));
    QVERIFY(entities.contains(QString::number(rmsEntityId)));
}

void test_recorder::createModule(int entityId, QMap<QString, QVariant> components)
{
    VfCpp::VfCppEntity * entity =new VfCpp::VfCppEntity(entityId);
    m_testRunner->getModManFacade()->addSubsystem(entity);
    entity->initModule();
    for(auto compoName : components.keys())
        entity->createComponent(compoName, components[compoName]);
    TimeMachineObject::feedEventLoop();
}

void test_recorder::fireActualValues()
{
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN1", 1);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN2", 2);
    m_testRunner->setVfComponent(rmsEntityId, "PAR_Interval", 5);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS1", 1);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS2", 2);
}

void test_recorder::triggerDftModuleSigMeasuring()
{
    //"SIG_Measuring" changes from 0 to 1 when new actual values are available
    m_testRunner->setVfComponent(sigMeasuringEntityId, "SIG_Measuring", QVariant(0));
    m_testRunner->setVfComponent(sigMeasuringEntityId, "SIG_Measuring", QVariant(1));
}
