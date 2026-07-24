#ifndef DEMOMODULEMANAGER_H
#define DEMOMODULEMANAGER_H

#include <modulemanager.h>
#include "abstractmockallservices.h"

class DemoModuleManager : public ZeraModules::ModuleManager
{
public:
    explicit DemoModuleManager(ModuleManagerSetupFacade *setupFacade,
                               const AbstractFactoryServiceInterfacesPtr &serviceInterfaceFactory,
                               const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                               QObject *parent = nullptr);
    void startAllDemoServices(const QString &deviceName);
private:
    std::unique_ptr<AbstractMockAllServices> m_mockAllServices;
};

#endif // DEMOMODULEMANAGER_H
