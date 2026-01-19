#ifndef TEST_RECORDER_H
#define TEST_RECORDER_H

#include "modulemanagertestrunner.h"
#include <vf_rpc_invoker.h>
#include <QObject>

class test_recorder : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void startLoggingOneRecording();
    void startLoggingMultipleRecordingsAndCheckTimestamps();
    void startStopRecordingTimerExpiredCheckResults();
    void startStopRecordingTimerExpired();
    void startStopTwoRecordingsNoTimerExpired();
    void startStopTwoRecordingsFirstRecordingTimerExpired();
    void startStopTwoRecordingsSecondRecordingTimerExpired();
    void startStopTwoRecordingsBothRecordingTimersExpired();

    void invokeRpcNoValuesRecorded();
    void invokeRpcOneValueRecorded();
    void invokeRpcMultipleValuesRecorded();

private:
    void createModule(int entityId, QMap<QString, QVariant> components);
    void fireActualValues();
    void triggerDftModuleSigMeasuring();
    void createModulesManually();
    QVariantMap callRecorderRpc(int start, int end);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    VfRPCInvokerPtr m_rpcClient;
};

#endif // TEST_RECORDER_H
