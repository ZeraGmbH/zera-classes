#ifndef TEST_ACT_VALUES_RMS_H
#define TEST_ACT_VALUES_RMS_H

#include "licensesystemmock.h"
#include "modulemanagersetupfacade.h"
#include "testmodulemanager.h"
#include <QObject>
#include <memory>

class test_act_values_rms : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void minimalSession();
private:
    void setupServices(QString sessionFileName);

    std::unique_ptr<LicenseSystemMock> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;
};

#endif // TEST_ACT_VALUES_RMS_H
