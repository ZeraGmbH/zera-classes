#ifndef TEST_MODMAN_WITH_VF_LOGGER_H
#define TEST_MODMAN_WITH_VF_LOGGER_H

#include "modulemanagertestrunner.h"
#include <vl_databaselogger.h>
#include <QObject>
#include <memory>

class test_modman_with_vf_logger : public QObject
{
    Q_OBJECT
private slots:
    void entitiesCreated();
    void loggerComponentsCreated();
    void contentSetsAvailable();
    void contentSetsSelectValid(); // This is a bug!!
    void contentSetsSelectInvalid();
    void contentSetsSelectValidList();

    void initTestCase();
    void init();
    void cleanup();
private:
    void onSerialNoLicensed();
    void createModmanWithLogger();
    void startModman(QString sessionFileName);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<VeinLogger::DatabaseLogger> m_dataLoggerSystem;
    VeinStorage::AbstractDatabase* m_storageDb = nullptr;
    bool m_dataLoggerSystemInitialized = false;
};

#endif // TEST_MODMAN_WITH_VF_LOGGER_H
