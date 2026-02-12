#ifndef TEST_RECORDER_REDUCER_H
#define TEST_RECORDER_REDUCER_H

#include "modulemanagertestrunner.h"
#include <vf_rpc_invoker.h>
#include <QObject>

class test_recorder_reducer : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void invokeRpcNoValuesRecorded();
    void invokeRpcFactorSimilarAsValuesRecorded();
    void invokeRpcFactorHigherThanValuesRecorded();
    void invokeRpcFactorLowerThanValuesRecorded();
    void invokeRpcFactorDoubleValuesRecorded();

private:
    void createModule(int entityId, QMap<QString, QVariant> components);
    void fireActualValues();
    void triggerDftModuleSigMeasuring();
    void createModulesManually();
    QVariantMap callRpc(QString RpcName, QVariantMap parameters);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    VfRPCInvokerPtr m_rpcClient;

};

#endif // TEST_RECORDER_REDUCER_H
