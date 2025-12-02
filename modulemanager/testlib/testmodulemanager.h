#ifndef TESTMODULEMANAGER_H
#define TESTMODULEMANAGER_H

#include <abstracttestallservices.h>
#include <modulemanager.h>
#include <mocktcpnetworkfactory.h>
#include <zdspserver.h>

class TestModuleManager : public ZeraModules::ModuleManager
{
public:
    static void enableTests();

    explicit TestModuleManager(ModuleManagerSetupFacade *setupFacade,
                               AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create(),
                               std::shared_ptr<QByteArray> configDataLastSaved = std::make_shared<QByteArray>());
    void startAllTestServices(QString deviceName, bool initialAdjPermission);
    void destroyModulesAndWaitUntilAllShutdown();
    void waitUntilModulesAreReady();
    ZeraModules::VirtualModule *getModule(QString uniqueName, int entityId);
    const QByteArray getLastStoredConfig();
    struct TModuleInstances {
        int m_veinMetaDataCount = 0;
        int m_veinComponentCount = 0;
        int m_veinEventSystemCount = 0;
        int m_activistCount = 0;
        int m_serviceInterfaceCount = 0;
        int m_dspVarCount = 0;

        int m_scpiNodes = 0;
        int m_scpiObjects = 0;

        int m_veinTcpPeerCount = 0;
    };
    QList<TModuleInstances> getInstanceCountsOnModulesDestroyed();

    ZDspServer *getDspServer();
    void setRangeGetSetDelay(int rangeGetSetDelay);

    // hotplug full functional methods
    void fireHotplugInterrupt(const AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap &deviceMap);
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap getCurrentHotplugMap() const;
    cSenseSettingsPtr getSenseSettings();

    // hotplug convenience methods
    void addStandardEmobControllers(const QStringList &channelAliases);
    void removeAllHotplugDevices();
    void addClamps(const QList<AbstractMockAllServices::clampParam> &clampParams);

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
    std::unique_ptr<AbstractTestAllServices> m_testAllServices;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
};

#endif // TESTMODULEMANAGER_H
