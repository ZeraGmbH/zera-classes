#ifndef SESSIONEXPORTGENERATOR_H
#define SESSIONEXPORTGENERATOR_H

#include "testlicensesystem.h"
#include "testmodulemanager.h"
#include "modulemanagerconfig.h"
#include <testdbaddsignaller.h>
#include <vl_databaselogger.h>
#include <QObject>

class SessionExportGenerator: public QObject
{
    Q_OBJECT
public:
    SessionExportGenerator(const LxdmSessionChangeParam &lxdmParam);
    ~SessionExportGenerator();

    void setDevice(QString device);
    QStringList getAvailableSessions();
    void changeSession(QString session);
    void generateDevIfaceXml(QString xmlDir);
    void generateSnapshotJsons(QString snapshotDir);
    QByteArray getVeinDump();
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();
private:
    void createModman(QString device);
    void destroyModules();
    void createXml(QString completeFileName, QString contents);
    void setVfComponent(int entityId, QString componentName, QVariant newValue);
    QVariant getVfComponent(int entityId, QString componentName);

    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    ModulemanagerConfig *m_modmanConfig;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanSetupFacade;
    std::unique_ptr<TestModuleManager> m_modman;
    QString m_device;
    std::unique_ptr<TestDbAddSignaller> m_testSignaller;
    std::unique_ptr<VeinLogger::DatabaseLogger> m_dataLoggerSystem;
    bool m_dataLoggerSystemInitialized = false;
    const LxdmSessionChangeParam m_lxdmParam;
};

#endif // SESSIONEXPORTGENERATOR_H
