#ifndef TEST_VEINDATACOLLECTOR_H
#define TEST_VEINDATACOLLECTOR_H

#include "veindatacollector.h"
#include "modulemanagertestrunner.h"
#include <vl_databaselogger.h>
#include <QObject>

class test_veinDataCollector : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void storeRmsValues();
    void storePowerValues();

private:
    void onSerialNoLicensed();
    void startModman(QString sessionFileName);
    QEvent *generateEvent(int entityId, QString componentName, QVariant oldValue, QVariant newValue);

    VeinDataCollector *m_dataCollector;
    VeinEvent::StorageSystem* m_storage;
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<VeinLogger::DatabaseLogger> m_dataLoggerSystem;
    bool m_dataLoggerSystemInitialized = false;
};

#endif // TEST_VEINDATACOLLECTOR_H
