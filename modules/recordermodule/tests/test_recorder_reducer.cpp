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

    int pointsNumber = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points").toInt();
    QCOMPARE(pointsNumber, 0);
    int pointsNumberInterpol1 = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points1").toInt();
    QCOMPARE(pointsNumberInterpol1, 0);
    int pointsNumberInterpol2 = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points2").toInt();
    QCOMPARE(pointsNumberInterpol2, 0);

    QVariantMap rpcParams;
    rpcParams.insert("p_startingPoint", 0);
    rpcParams.insert("p_endingPoint", 1);
    QVariantMap errorMap = callRpc("RPC_GetRecordedValuesReduced1", rpcParams);
    QVariant errorMsg = errorMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(!errorMsg.toString().isEmpty());
    QVariant result = errorMap[VeinComponent::RemoteProcedureData::s_returnString];
    QVERIFY(result.toString().isEmpty());

    errorMap = callRpc("RPC_GetRecordedValuesReduced2", rpcParams);
    errorMsg = errorMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(!errorMsg.toString().isEmpty());
    result = errorMap[VeinComponent::RemoteProcedureData::s_returnString];
    QVERIFY(result.toString().isEmpty());
}

void test_recorder_reducer::invokeRpcOneValueRecorded()
{
    createModulesManually();
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(500);

    int pointsNumber = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points").toInt();
    QCOMPARE(pointsNumber, 1);
    int pointsNumberInterpol1 = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points1").toInt();
    QCOMPARE(pointsNumberInterpol1, 0);
    int pointsNumberInterpol2 = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points2").toInt();
    QCOMPARE(pointsNumberInterpol2, 0);

    QVariantMap rpcParams;
    rpcParams.insert("p_startingPoint", 0);
    rpcParams.insert("p_endingPoint", 1);
    QVariantMap errorMap = callRpc("RPC_GetRecordedValuesReduced1", rpcParams);
    QVariant errorMsg = errorMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(!errorMsg.toString().isEmpty());
    QVariant result = errorMap[VeinComponent::RemoteProcedureData::s_returnString];
    QVERIFY(result.toString().isEmpty());

    errorMap = callRpc("RPC_GetRecordedValuesReduced2", rpcParams);
    errorMsg = errorMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(!errorMsg.toString().isEmpty());
    result = errorMap[VeinComponent::RemoteProcedureData::s_returnString];
    QVERIFY(result.toString().isEmpty());
}

void test_recorder_reducer::invokeRpcFirstStack()
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

    int pointsNumber = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points").toInt();
    QCOMPARE(pointsNumber, 4);
    int pointsNumberInterpol1 = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points1").toInt();
    QCOMPARE(pointsNumberInterpol1, 1);
    int pointsNumberInterpol2 = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points2").toInt();
    QCOMPARE(pointsNumberInterpol2, 0);

    QVariantMap rpcParams;
    rpcParams.insert("p_startingPoint", 0);
    rpcParams.insert("p_endingPoint", 1);
    QVariantMap argMap = callRpc("RPC_GetRecordedValuesReduced1", rpcParams);
    QVariant errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(errorMsg.toString().isEmpty());
    QJsonObject result = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QVERIFY(result.size()==1);
    QJsonValue value = result.value(result.keys().at(0));
    QStringList entities = value.toObject().keys();
    QVERIFY(entities.size() == 2);
    QVERIFY(entities.contains(QString::number(powerEntityId)));
    QVERIFY(entities.contains(QString::number(rmsEntityId)));

    argMap = callRpc("RPC_GetRecordedValuesReduced2", rpcParams);
    errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(!errorMsg.toString().isEmpty());
}

void test_recorder_reducer::invokeRpcFirstStackSecondStack()
{
    createModulesManually();
    m_testRunner->setVfComponent(recorderEntityId, "PAR_StartStopRecording", true);
    constexpr int actualValueCount = 16;
    constexpr int measPeriodMs = 500;
    for (int valueNo=0; valueNo<actualValueCount; valueNo++) {
        fireActualValues();
        triggerDftModuleSigMeasuring();
        TimeMachineForTest::getInstance()->processTimers(measPeriodMs);
    }

    int pointsNumber = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points").toInt();
    QCOMPARE(pointsNumber, 16);
    int pointsNumberInterpol1 = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points1").toInt();
    QCOMPARE(pointsNumberInterpol1, 4);
    int pointsNumberInterpol2 = m_testRunner->getVfComponent(recorderEntityId, "ACT_Points2").toInt();
    QCOMPARE(pointsNumberInterpol2, 1);

    QVariantMap rpcParams;
    rpcParams.insert("p_startingPoint", 0);
    rpcParams.insert("p_endingPoint", 4);
    QVariantMap argMap = callRpc("RPC_GetRecordedValuesReduced1", rpcParams);
    QVariant errorMsg = argMap[VeinComponent::RemoteProcedureData::s_errorMessageString];
    QVERIFY(errorMsg.toString().isEmpty());
    QJsonObject result = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QVERIFY(result.size()==4); // 4 timestamps

    rpcParams.clear();
    rpcParams.insert("p_startingPoint", 0);
    rpcParams.insert("p_endingPoint", 1);
    argMap = callRpc("RPC_GetRecordedValuesReduced2", rpcParams);

    result = argMap[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
    QVERIFY(result.size()==1);  //one timestamp
    QJsonValue value = result.value(result.keys().at(0));
    QStringList entities = value.toObject().keys();
    QVERIFY(entities.size() == 2);
    QVERIFY(entities.contains(QString::number(powerEntityId)));
    QVERIFY(entities.contains(QString::number(rmsEntityId)));
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

