#ifndef SCPIIFACEEXPORTGENERATOR_H
#define SCPIIFACEEXPORTGENERATOR_H

#include "licensesystemmock.h"
#include "testmodulemanager.h"
#include "modulemanagerconfig.h"

class ScpiIfaceExportGenerator
{
public:
    ScpiIfaceExportGenerator();
    ~ScpiIfaceExportGenerator();
    QString getSessionScpiIface(QString device, QString session);
    void getAllSessionsScpiIfaceXmls(QString device, QString xmlDir);
private:
    void setDevice(QString device);
    void shutdownModules();
    void createXml(QString completeFileName, QString contents);
    LicenseSystemMock m_licenseSystem;
    ModulemanagerConfig *m_modmanConfig;
    ModuleManagerSetupFacade *m_modmanSetupFacade;
    TestModuleManager *m_modman;
    QString m_device;
};

#endif // SCPIIFACEEXPORTGENERATOR_H
