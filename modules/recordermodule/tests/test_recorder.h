#ifndef TEST_RECORDER_H
#define TEST_RECORDER_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_recorder : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void startLoggingOneRecording();
    void startLoggingMultipleRecordings();
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

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;

};

#endif // TEST_RECORDER_H
