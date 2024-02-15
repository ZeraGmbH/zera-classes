#ifndef TEST_REGRESSION_VEIN_MODULE_RMS_H
#define TEST_REGRESSION_VEIN_MODULE_RMS_H

#include "licensesystemmock.h"
#include "modulemanagersetupfacade.h"
#include "testmodulemanager.h"
#include <QObject>
#include <memory>

class test_regression_vein_module_rms : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
private:
    void setupServices(QString sessionFileName);

    std::unique_ptr<LicenseSystemMock> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
};

#endif // TEST_REGRESSION_VEIN_MODULE_RMS_H
