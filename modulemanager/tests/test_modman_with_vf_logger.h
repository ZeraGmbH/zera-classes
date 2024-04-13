#ifndef TEST_MODMAN_WITH_VF_LOGGER_H
#define TEST_MODMAN_WITH_VF_LOGGER_H

#include "modulemanagertestrunner.h"
#include "veinqml.h"
#include "zeradblogger.h"
#include <vsc_scriptsystem.h>
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
    void onVfQmlStateChanged(VeinApiQml::VeinQml::ConnectionState t_state);
    void onSerialNoLicensed();
    void createModmanWithLogger();
    void startModman(QString sessionFileName);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<VeinScript::ScriptSystem> m_scriptSystem;
    std::unique_ptr<VeinApiQml::VeinQml> m_qmlSystem;
    std::unique_ptr<ZeraDBLogger> m_dataLoggerSystem;
    VeinEvent::StorageSystem* m_storage;
    bool m_initQmlSystemOnce = false;
    bool m_dataLoggerSystemInitialized = false;
};

#endif // TEST_MODMAN_WITH_VF_LOGGER_H
