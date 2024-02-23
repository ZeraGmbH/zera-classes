#ifndef TEST_FFT_MODULE_REGRESSION_H
#define TEST_FFT_MODULE_REGRESSION_H

#include "licensesystemmock.h"
#include "modulemanagersetupfacade.h"
#include "testfactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include <QObject>
#include <memory>

class test_fft_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectValues();
    /*void injectActualValuesNoReferenceChannel();
    void injectActualValuesReferenceChannelUL1();
    void injectActualValuesReferenceChannelUL2();
    void injectActualValuesOrder0();Ü*/
private:
    void setupServices(QString sessionFileName);

    std::unique_ptr<LicenseSystemMock> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;

    TestFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
};

#endif // TEST_FFT_MODULE_REGRESSION_H
