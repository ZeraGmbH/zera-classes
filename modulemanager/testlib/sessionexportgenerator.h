#ifndef SESSIONEXPORTGENERATOR_H
#define SESSIONEXPORTGENERATOR_H

#include "testlicensesystem.h"
#include "testmodulemanager.h"
#include "modulemanagerconfig.h"

class SessionExportGenerator
{
public:
    SessionExportGenerator(const LxdmSessionChangeParam &lxdmParam);
    ~SessionExportGenerator();

    void setDevice(QString device);
    QStringList getAvailableSessions();
    void changeSession(QString session);
    void generateDevIfaceXml(QString xmlDir);
    QByteArray getVeinDump();
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();
private:
    void createModman(QString device);
    void destroyModules();
    void createXml(QString completeFileName, QString contents);

    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    ModulemanagerConfig *m_modmanConfig;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanSetupFacade;
    std::unique_ptr<TestModuleManager> m_modman;
    QString m_device;
    const LxdmSessionChangeParam m_lxdmParam;
};

#endif // SESSIONEXPORTGENERATOR_H
