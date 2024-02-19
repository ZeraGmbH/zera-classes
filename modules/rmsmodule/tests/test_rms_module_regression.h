#ifndef TEST_RMS_MODULE_REGRESSION_H
#define TEST_RMS_MODULE_REGRESSION_H

#include "licensesystemmock.h"
#include "modulemanagersetupfacade.h"
#include "testmodulemanager.h"
#include <QObject>
#include <memory>

class test_rms_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectActualValues();
    void injectActualTwice();
private:
    void setupServices(QString sessionFileName);

    std::unique_ptr<LicenseSystemMock> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
};

#endif // TEST_RMS_MODULE_REGRESSION_H
