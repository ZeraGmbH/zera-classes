#include "modulemanagertestrunner.h"
#include <timemachineobject.h>

ModuleManagerTestRunner::ModuleManagerTestRunner(QString sessionFileName, QString deviceName)
{
    m_licenseSystem = std::make_unique<LicenseSystemMock>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), m_serviceInterfaceFactory);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllServiceMocks(deviceName);
    m_modMan->loadSession(sessionFileName);
    m_modMan->waitUntilModulesAreReady();
    m_vfCmdEventHandlerSystem = std::make_shared<VfCmdEventHandlerSystem>();
    m_modmanFacade->addSubsystem(m_vfCmdEventHandlerSystem.get());
}

ModuleManagerTestRunner::~ModuleManagerTestRunner()
{
    if(m_modMan)
        m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_modMan = nullptr;
    TimeMachineObject::feedEventLoop();
    m_serviceInterfaceFactory = nullptr;
    m_modmanFacade = nullptr;
    m_licenseSystem = nullptr;
}

VeinStorage::VeinHash *ModuleManagerTestRunner::getVeinStorageSystem()
{
    return m_modmanFacade->getStorageSystem();
}

const QList<TestDspInterfacePtr> &ModuleManagerTestRunner::getDspInterfaceList() const
{
    return m_serviceInterfaceFactory->getInterfaceList();
}

VfCmdEventHandlerSystemPtr ModuleManagerTestRunner::getVfCmdEventHandlerSystemPtr()
{
    return m_vfCmdEventHandlerSystem;
}

ZeraModules::VirtualModule *ModuleManagerTestRunner::getModule(QString uniqueName, int entityId)
{
    return m_modMan->getModule(uniqueName, entityId);
}
