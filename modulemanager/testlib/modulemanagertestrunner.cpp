#include "modulemanagertestrunner.h"
#include "vf_client_component_setter.h"
#include <timemachineobject.h>

ModuleManagerTestRunner::ModuleManagerTestRunner(QString sessionFileName, bool initialAdjPermission, QString deviceName, LxdmSessionChangeParam lxdmParam)
{
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get(),
                                                                false,
                                                                lxdmParam);
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), m_serviceInterfaceFactory);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllTestServices(deviceName, initialAdjPermission);
    m_vfCmdEventHandlerSystem = std::make_shared<VfCmdEventHandlerSystem>();
    m_modmanFacade->addSubsystem(m_vfCmdEventHandlerSystem.get());
    if(!sessionFileName.isEmpty())
        start(sessionFileName);
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

void ModuleManagerTestRunner::start(QString sessionFileName)
{
    m_modMan->loadSession(sessionFileName);
    m_modMan->waitUntilModulesAreReady();
}

VeinStorage::AbstractEventSystem *ModuleManagerTestRunner::getVeinStorageSystem()
{
    return m_modmanFacade->getStorageSystem();
}

TestDspInterfacePtr ModuleManagerTestRunner::getDspInterface(int entityId,
                                                             TestFactoryServiceInterfaces::DSPInterfaceType dspInterfaceType)
{
    return m_serviceInterfaceFactory->getInterface(entityId, dspInterfaceType);
}

const QList<TestDspInterfacePtr> &ModuleManagerTestRunner::getDspInterfaceList() const
{
    return m_serviceInterfaceFactory->getInterfaceList();
}

VfCmdEventHandlerSystemPtr ModuleManagerTestRunner::getVfCmdEventHandlerSystemPtr()
{
    return m_vfCmdEventHandlerSystem;
}

ModuleManagerSetupFacade *ModuleManagerTestRunner::getModManFacade()
{
    return m_modmanFacade.get();
}

ZeraModules::VirtualModule *ModuleManagerTestRunner::getModule(QString uniqueName, int entityId)
{
    return m_modMan->getModule(uniqueName, entityId);
}

void ModuleManagerTestRunner::setVfComponent(int entityId, QString componentName, QVariant newValue)
{
    QVariant oldValue = getVeinStorageSystem()->getDb()->getStoredValue(entityId, componentName);
    QEvent* event = VfClientComponentSetter::generateEvent(entityId, componentName, oldValue, newValue);
    emit m_modmanFacade->getStorageSystem()->sigSendEvent(event); // could be any event system
    TimeMachineObject::feedEventLoop();
}

QVariant ModuleManagerTestRunner::getVfComponent(int entityId, QString componentName)
{
    return getVeinStorageSystem()->getDb()->getStoredValue(entityId, componentName);
}
