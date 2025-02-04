#ifndef TEST_RANGE_AUTOMATIC_H
#define TEST_RANGE_AUTOMATIC_H

#include "testfactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include <testlicensesystem.h>
#include <QObject>

class test_range_automatic : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void defaultRangesAndSetting();
    void testRangeAutomatic();
    void enableAndDisableRangeAutomatic();
    void softOverloadWithRangeAutomatic();
private:
    void fireNewRmsValues(float rmsValue);
    void setVfComponent(int entityId, QString componentName, QVariant newValue);
    QVariant getVfComponent(int entityId, QString componentName);

    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanSetupFacade;
    TestFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    std::unique_ptr<TestModuleManager> m_modMan;
    TestDspInterfacePtr m_rangeModuleMeasProgramDspInterface;
};

#endif // TEST_RANGE_AUTOMATIC_H
