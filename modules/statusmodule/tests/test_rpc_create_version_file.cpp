#include "test_rpc_create_version_file.h"
#include "vcmp_remoteproceduredata.h"
#include <timemachineobject.h>
#include <vf_client_rpc_invoker.h>
#include <QSignalSpy>
#include <QFile>
#include <QTest>

QTEST_MAIN(test_rpc_create_version_file)

static int constexpr statusEntityId = 1150;
static const QString destinationPath = QStringLiteral("/tmp/zenux-version.json");

void test_rpc_create_version_file::init()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/status-min-session.json");
    m_rpcInvoker = std::make_shared<VfRPCInvoker>(statusEntityId, std::make_unique<VfClientRPCInvoker>());
    m_testRunner->getVfCmdEventHandlerSystemPtr()->addItem(m_rpcInvoker);
}

void test_rpc_create_version_file::cleanup()
{
    m_testRunner = nullptr;
    QFile::remove(destinationPath);
}

void test_rpc_create_version_file::createVersionFile()
{
    QList<int> entityList = m_testRunner->getVeinStorageDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);

    QSignalSpy spyRpcFinish(m_rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished);
    QVariantMap parameters;
    parameters["p_destinationPath"] = destinationPath;
    m_rpcInvoker->invokeRPC("RPC_CreateVersionFile", parameters);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRpcFinish.count(), 1);

    QVariantMap argMap = spyRpcFinish[0][1].toMap();
    QCOMPARE(argMap[VeinComponent::RemoteProcedureData::s_resultCodeString], VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);
    QFile file(destinationPath);
    QVERIFY(file.exists());
    QVERIFY(file.size() != 0);
}

void test_rpc_create_version_file::createVersionFileTwice()
{
    QSignalSpy spyRpcFinish(m_rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished);
    QVariantMap parameters;
    parameters["p_destinationPath"] = destinationPath;
    m_rpcInvoker->invokeRPC("RPC_CreateVersionFile", parameters);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRpcFinish.count(), 1);
    m_rpcInvoker->invokeRPC("RPC_CreateVersionFile", parameters);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRpcFinish.count(), 2);

    QVariantMap argMap = spyRpcFinish[0][1].toMap();
    QCOMPARE(argMap[VeinComponent::RemoteProcedureData::s_resultCodeString], VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS);
    QFile file(destinationPath);
    QVERIFY(file.exists());
    QVERIFY(file.size() != 0);
}
