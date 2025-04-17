#ifndef TEST_RANGE_AUTOMATIC_H
#define TEST_RANGE_AUTOMATIC_H

#include "testfactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include <testserverforsenseinterfacemt310s2.h>
#include <mockzdsp1d.h>
#include <resmanrunfacade.h>
#include <testlicensesystem.h>
#include <QObject>


class test_range_automatic : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();
    void defaultRangesAndSetting();
    void activeGroupingChangeSingleRange();
    void testRangeAutomatic();
    void testRangeAutomaticIncreaseU();
    void testRangeAutomaticDecreaseU();
    void testRangeAutomaticIncreaseLowRangesI();
    void testRangeAutomaticIncreaseHighRangesI();
    void testRangeAutomaticDecreaseI();

    void checkRmsOverload();

    void enableAndDisableRangeAutomatic();
    void softOverloadWithRangeAutomatic();
    void addAndSelectClamp();
    void selectClampThenRangeAutomatic();
    void addRemoveClamp();
    void checkPersitency();
private:
    void setupServices();
    void fireNewRmsValues(float rmsValue, bool includeIAux);
    void setVfComponent(int entityId, QString componentName, QVariant newValue);
    QVariant getVfComponent(int entityId, QString componentName);
    QStringList getCurrentRanges();

    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanSetupFacade;
    TestFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    std::unique_ptr<TestModuleManager> m_modMan;
    TestDspInterfacePtr m_rangeModuleMeasProgramDspInterface;

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testPcbServer;
    std::unique_ptr<MockZdsp1d> m_dspServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    std::shared_ptr<QByteArray> m_configDataLastStored;
    static constexpr bool withIaux = true;
    static constexpr bool withoutIaux = false;
};

#endif // TEST_RANGE_AUTOMATIC_H
