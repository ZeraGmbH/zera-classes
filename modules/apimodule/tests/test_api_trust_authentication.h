#ifndef TEST_API_TRUST_AUTHENTICATION_H
#define TEST_API_TRUST_AUTHENTICATION_H

#include <QObject>

#include "testlicensesystem.h"
#include "testfactoryserviceinterfaces.h"
#include "testmodulemanager.h"

class test_api_trust_authentication : public QObject
{
    Q_OBJECT

private:
    std::shared_ptr<QByteArray> m_configDataLastStored;

    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanSetupFacade;
    TestFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    std::unique_ptr<TestModuleManager> m_modMan;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;

private slots:
    void rpcRequestWillTriggerGUI();
    void initTestCase();
    void init();
};

#endif // TEST_API_TRUST_AUTHENTICATION_H
