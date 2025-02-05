#ifndef TEST_RANGE_AUTOMATIC_H
#define TEST_RANGE_AUTOMATIC_H

#include "testfactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include <mockmt310s2d.h>
#include <mocksec1000d.h>
#include <mockzdsp1d.h>
#include <resmanrunfacade.h>
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
    void setupServices();
    void fireNewRmsValues(float rmsValue);
    void setVfComponent(int entityId, QString componentName, QVariant newValue);
    QVariant getVfComponent(int entityId, QString componentName);

    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanSetupFacade;
    TestFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    std::unique_ptr<TestModuleManager> m_modMan;
    TestDspInterfacePtr m_rangeModuleMeasProgramDspInterface;

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<MockMt310s2d> m_testPcbServer;
    std::unique_ptr<MockSec1000d> m_secServer;
    std::unique_ptr<MockZdsp1d> m_dspServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
};

#endif // TEST_RANGE_AUTOMATIC_H
