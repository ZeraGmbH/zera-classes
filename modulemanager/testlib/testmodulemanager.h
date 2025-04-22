#ifndef TESTMODULEMANAGER_H
#define TESTMODULEMANAGER_H

#include <modulemanager.h>
#include <tcpnetworkfactory.h>

class TestModuleManager : public ZeraModules::ModuleManager
{
public:
    static void enableTests();

    explicit TestModuleManager(ModuleManagerSetupFacade *setupFacade,
                               AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create(),
                               std::shared_ptr<QByteArray> configDataLastSaved = std::make_shared<QByteArray>());
    void startAllTestServices(QString deviceName, bool initialAdjPermission);
    void destroyModulesAndWaitUntilAllShutdown();
    void waitUntilModulesAreReady();
    ZeraModules::VirtualModule *getModule(QString uniqueName, int entityId);
    const QByteArray getLastStoredConfig();
    VeinTcp::AbstractTcpNetworkFactoryPtr getTcpNetworkFactory();
    struct TModuleInstances {
        int m_componentCount = 0;
        int m_validatorCount = 0;
        int m_commandFilterCount = 0;
    };
    QList<TModuleInstances> getInstanceCountsOnModulesDestroyed();

private slots:
    void onAllModulesDestroyed();

private:
    static bool prepareOe();
    static void pointToInstalledSessionFiles();
    bool modulesReady();
    QStringList getModuleFileNames() override;
    void saveModuleConfig(ZeraModules::ModuleData *moduleData) override;
    // Avoid file write in install area on tests (OE tests turn 'ff')
    void saveDefaultSession() override {};

    std::shared_ptr<QByteArray> m_configDataLastSaved;
    QList<TModuleInstances> m_instantCountsOnModulesDestroyed;
};

#endif // TESTMODULEMANAGER_H
