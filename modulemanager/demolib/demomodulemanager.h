#ifndef DEMOMODULEMANAGER_H
#define DEMOMODULEMANAGER_H

#include <modulemanager.h>
#include "abstractmockallservices.h"

class DemoModuleManager : public ZeraModules::ModuleManager
{
public:
    explicit DemoModuleManager(ModuleManagerSetupFacade *setupFacade,
                               AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                               QObject *parent = nullptr);
    void startAllDemoServices(QString deviceName);
private:
    std::unique_ptr<AbstractMockAllServices> m_mockAllServices;
};

#endif // DEMOMODULEMANAGER_H
