#ifndef SCPIIFACEEXPORTGENERATOR_H
#define SCPIIFACEEXPORTGENERATOR_H

#include "testlicensesystem.h"
#include "testmodulemanager.h"
#include "modulemanagerconfig.h"

class ScpiIfaceExportGenerator
{
public:
    ScpiIfaceExportGenerator();
    ~ScpiIfaceExportGenerator();
    void getAllSessionsScpiIfaceXmls(QString device, QString xmlDir);
private:
    void createModman(QString device);
    void destroyModules();
    QString getSessionScpiIface(QString session);
    void setDevice(QString device);
    void createXml(QString completeFileName, QString contents);
    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    ModulemanagerConfig *m_modmanConfig;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanSetupFacade;
    std::unique_ptr<TestModuleManager> m_modman;
    QString m_device;
};

#endif // SCPIIFACEEXPORTGENERATOR_H
