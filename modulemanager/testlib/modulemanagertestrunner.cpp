#include "modulemanagertestrunner.h"
#include "vf_client_component_setter.h"
#include "vf_entity_component_event_item.h"
#include <timemachineobject.h>
#include <vtcp_workerfactorymethodstest.h>

ModuleManagerTestRunner::ModuleManagerTestRunner(QString sessionFileName, bool initialAdjPermission, QString deviceName)
{
    VeinTcp::TcpWorkerFactoryMethodsTest::enableMockNetwork();
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), m_serviceInterfaceFactory);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllTestServices(deviceName, initialAdjPermission);
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
    m_vfCmdEventHandlerSystem = std::make_shared<VfCmdEventHandlerSystem>();
    m_modmanFacade->addSubsystem(m_vfCmdEventHandlerSystem.get());
}

VeinEvent::StorageSystem *ModuleManagerTestRunner::getVeinStorageSystem()
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

ModuleManagerSetupFacade *ModuleManagerTestRunner::getModManFacade()
{
    return m_modmanFacade.get();
}

ZeraModules::VirtualModule *ModuleManagerTestRunner::getModule(QString uniqueName, int entityId)
{
    return m_modMan->getModule(uniqueName, entityId);
}

void ModuleManagerTestRunner::setVfComponent(int moduleId, QString componentName, QVariant oldValue, QVariant newValue)
{
    VfCmdEventItemEntityPtr entityItem = VfEntityComponentEventItem::create(moduleId);
    m_vfCmdEventHandlerSystem->addItem(entityItem);

    VfClientComponentSetterPtr setter = VfClientComponentSetter::create(componentName, entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent(oldValue, newValue);
    TimeMachineObject::feedEventLoop();
}
