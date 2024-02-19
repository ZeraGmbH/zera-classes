#ifndef TEST_DFT_MODULE_REGRESSION_H
#define TEST_DFT_MODULE_REGRESSION_H

#include "licensesystemmock.h"
#include "modulemanagersetupfacade.h"
#include "testmodulemanager.h"
#include "vf_cmd_event_handler_system.h"
#include <QObject>
#include <memory>

class test_dft_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectActualValuesNoReferenceChannel();
    void injectActualValuesReferenceChannelUL1();
    void injectActualValuesReferenceChannelUL2();
    void injectActualValuesOrder0();
private:
    void setupServices(QString sessionFileName);
    void setReferenceChannel(QString channel);

    std::unique_ptr<LicenseSystemMock> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;

    VfCmdEventHandlerSystemPtr m_vfCmdEventHandlerSystem;
};

#endif // TEST_DFT_MODULE_REGRESSION_H
