#ifndef TEST_SCPI_QUEUE_H
#define TEST_SCPI_QUEUE_H

#include "licensesystemmock.h"
#include "modulemanagersetupfacade.h"
#include "demofactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include <QObject>
#include <memory>

class test_scpi_queue : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void sendStandardCmdsQueueDisabledAndEnabled();
    void sendErroneousAndCorrectStandardCmds();

    void sendSubSystemAndStandardCommands();
    void enableAndDisableQueueWhileExecutingCmds();
    void disableAndEnableQueueWhileExecutingCmds();
private:
    QByteArray loadConfig(QString configFileNameFull);

    void setupServices(QString sessionFileName);
    void setReferenceChannel(QString channel);

    std::unique_ptr<LicenseSystemMock> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    std::unique_ptr<TestModuleManager> m_modMan;

    std::shared_ptr<DemoFactoryServiceInterfaces> m_serviceInterfaceFactory;
};

#endif // TEST_SCPI_QUEUE_H
