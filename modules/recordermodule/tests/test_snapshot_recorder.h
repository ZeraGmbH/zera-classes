#ifndef TEST_SNAPSHOT_RECORDER_H
#define TEST_SNAPSHOT_RECORDER_H

#include "modulemanagertestrunner.h"
#include <QObject>
#include <vf_rpc_invoker.h>

class test_snapshot_recorder : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void takeSnapshotNoValuesCsvEmpty();
    void takeSnapshotOneValueCsv();
    void takeSnapshotMaximumValuesCsvFull();

private:
    void createModmanWithLogger();
    void createModulesManually();
    void createModule(int entityId, QMap<QString, QVariant> components);
    void fireActualValues();
    void triggerDftModuleSigMeasuring();
    void takeSnapshot(QString sessionName, QString transactionName);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    VeinStorage::AbstractDatabase* m_storageDb = nullptr;
    VfRPCInvokerPtr m_rpcClient;
};

#endif // TEST_SNAPSHOT_RECORDER_H
