#ifndef TEST_DFT_MODULE_REGRESSION_H
#define TEST_DFT_MODULE_REGRESSION_H

#include "licensesystemmock.h"
#include "modulemanagersetupfacade.h"
#include "testfactoryactvalmaninthemiddle.h"
#include "testmodulemanager.h"
#include <QObject>
#include <memory>

class test_dft_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectActualValues();
private:
    void setupServices(QString sessionFileName);

    std::unique_ptr<LicenseSystemMock> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
    TestFactoryActValManInTheMiddlePtr m_factoryActualValueGen;
};

#endif // TEST_DFT_MODULE_REGRESSION_H
