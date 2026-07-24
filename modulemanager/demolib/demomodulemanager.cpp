#include "demomodulemanager.h"
#include "demoallservicescom5003.h"
#include "demoallservicesmt310s2.h"

DemoModuleManager::DemoModuleManager(ModuleManagerSetupFacade *setupFacade,
                                     const AbstractFactoryServiceInterfacesPtr &serviceInterfaceFactory,
                                     const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                                     QObject *parent) :
    ZeraModules::ModuleManager(setupFacade,
                               serviceInterfaceFactory,
                               tcpNetworkFactory,
                               true,
                               parent)
{

}

void DemoModuleManager::startAllDemoServices(const QString &deviceName)
{
    if (m_mockAllServices)
        m_mockAllServices = nullptr;
    if(deviceName == "mt310s2" || deviceName == "mt581s2")
        m_mockAllServices = std::make_unique<DemoAllServicesMt310s2>(deviceName);
    else if(deviceName == "com5003")
        m_mockAllServices = std::make_unique<DemoAllServicesCom5003>();
}
