#include "test_recorder_reducer.h"
#include <vf_client_rpc_invoker.h>
#include <vf-cpp-entity.h>
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <testloghelpers.h>
#include <QTest>
#include <QSignalSpy>

QTEST_MAIN(test_recorder_reducer)

static constexpr int sigMeasuringEntityId = 1050; //DftModule
constexpr int rmsEntityId = 1040;
constexpr int powerEntityId = 1070;
constexpr int recorderEntityId = 1800;

void test_recorder_reducer::init()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/mt310s2-emob-session-ac.json");
}

void test_recorder_reducer::cleanup()
{
    m_testRunner = nullptr;
    m_rpcClient = nullptr;
}

void test_recorder_reducer::invokeRpcNoValuesRecorded()
{
    createModulesManually();

    QVariantMap errorMap = callRpc("RPC_GetRecordedDataSampler", QVariantMap());
    QVariant errorMsg = errorMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(!errorMsg.toString().isEmpty());
    QVariant result = errorMap[VeinComponent::RemoteProcedureData::s_returnString];
    QVERIFY(result.toString().isEmpty());
}

void test_recorder_reducer::invokeRpcFactorSimilarAsValuesRecorded()
{
    createModulesManually();
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    constexpr int actualValueCount = 4;
    constexpr int measPeriodMs = 500;
    for (int valueNo=0; valueNo<actualValueCount; valueNo++) {
        fireActualValues();
        triggerDftModuleSigMeasuring();
        TimeMachineForTest::getInstance()->processTimers(measPeriodMs);
    }

    QVariantMap argMap = callRpc("RPC_GetRecordedDataSampler", QVariantMap());
    QVariant errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(errorMsg.toString().isEmpty());

    QJsonObject result = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QByteArray jsonDumped = TestLogHelpers::dump(result);
    QString jsonExpected = TestLogHelpers::loadFile(":/ReducedJsonFactorSimilarAsValuesRecorded.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_recorder_reducer::invokeRpcFactorHigherThanValuesRecorded()
{
    createModulesManually();
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    constexpr int actualValueCount = 6;
    constexpr int measPeriodMs = 500;
    for (int valueNo=0; valueNo<actualValueCount; valueNo++) {
        fireActualValues();
        triggerDftModuleSigMeasuring();
        TimeMachineForTest::getInstance()->processTimers(measPeriodMs);
    }

    QVariantMap argMap = callRpc("RPC_GetRecordedDataSampler", QVariantMap());
    QVariant errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(errorMsg.toString().isEmpty());

    QJsonObject result = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QByteArray jsonDumped = TestLogHelpers::dump(result);
    QString jsonExpected = TestLogHelpers::loadFile(":/ReducedJsonFactorHigherThanValuesRecorded.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_recorder_reducer::invokeRpcFactorLowerThanValuesRecorded()
{
    createModulesManually();
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    constexpr int actualValueCount = 3;
    constexpr int measPeriodMs = 500;
    for (int valueNo=0; valueNo<actualValueCount; valueNo++) {
        fireActualValues();
        triggerDftModuleSigMeasuring();
        TimeMachineForTest::getInstance()->processTimers(measPeriodMs);
    }

    QVariantMap argMap = callRpc("RPC_GetRecordedDataSampler", QVariantMap());
    QVariant errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(!errorMsg.toString().isEmpty());
}

void test_recorder_reducer::invokeRpcFactorDoubleValuesRecorded()
{
    createModulesManually();
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    constexpr int actualValueCount = 8;
    constexpr int measPeriodMs = 500;
    for (int valueNo=0; valueNo<actualValueCount; valueNo++) {
        fireActualValues();
        triggerDftModuleSigMeasuring();
        TimeMachineForTest::getInstance()->processTimers(measPeriodMs);
    }

    QVariantMap argMap = callRpc("RPC_GetRecordedDataSampler", QVariantMap());
    QVariant errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(errorMsg.toString().isEmpty());

    QJsonObject result = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QByteArray jsonDumped = TestLogHelpers::dump(result);
    QString jsonExpected = TestLogHelpers::loadFile(":/ReducedJsonFactorDoubleValuesRecorded.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_recorder_reducer::createModule(int entityId, QMap<QString, QVariant> components)
{
    VfCpp::VfCppEntity * entity =new VfCpp::VfCppEntity(entityId);
    m_testRunner->getModManFacade()->addSubsystem(entity);
    entity->initModule();
    for(const auto &compoName : components.keys())
        entity->createComponent(compoName, components[compoName]);
    TimeMachineObject::feedEventLoop();
}

void test_recorder_reducer::fireActualValues()
{
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN1", 1);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN2", 2);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS1", 1);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS2", 2);
}

void test_recorder_reducer::triggerDftModuleSigMeasuring()
{
    //"SIG_Measuring" changes from 0 to 1 when new actual values are available
    m_testRunner->setVfComponent(sigMeasuringEntityId, "SIG_Measuring", QVariant(0));
    m_testRunner->setVfComponent(sigMeasuringEntityId, "SIG_Measuring", QVariant(1));
}

void test_recorder_reducer::createModulesManually()
{
    QVariantMap components = {{"ACT_RMSPN1", QVariant()}, {"ACT_RMSPN2", QVariant()}};
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {{"ACT_PQS1", QVariant()}, {"ACT_PQS2", QVariant()}};
    createModule(powerEntityId, components);
}

QVariantMap test_recorder_reducer::callRpc(QString RpcName, QVariantMap parameters)
{
    m_rpcClient = std::make_shared<VfRPCInvoker>(recorderEntityId, std::make_unique<VfClientRPCInvoker>());
    m_testRunner->getVfCmdEventHandlerSystemPtr()->addItem(m_rpcClient);
    QSignalSpy spy(m_rpcClient.get(), &VfRPCInvoker::sigRPCFinished);
    m_rpcClient->invokeRPC(RpcName, parameters);
    TimeMachineObject::feedEventLoop();
    return spy[0][1].toMap();
}

