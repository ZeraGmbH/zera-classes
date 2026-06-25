#ifndef MODULEMANAGERTESTRUNNER_H
#define MODULEMANAGERTESTRUNNER_H

#include "testlicensesystem.h"
#include "modulemanagersetupfacade.h"
#include "testdspinterface.h"
#include "testfactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include "vf_cmd_event_handler_system.h"
#include <testdbaddsignaller.h>
#include <vl_databaselogger.h>
#include <mocklxdmsessionchangeparamgenerator.h>
#include <QObject>
#include <memory>

class ModuleManagerTestRunner : public QObject
{
    Q_OBJECT
public:
    explicit ModuleManagerTestRunner(const QString &sessionFileName,
                                     bool initialAdjPermission = false,
                                     const QString &deviceName = "mt310s2",
                                     const LxdmSessionChangeParam &lxdmParam = MockLxdmSessionChangeParamGenerator::generateTestSessionChanger(),
                                     bool addVfLogger = false,
                                     bool cleanupLxdmFiles = true);
    ~ModuleManagerTestRunner();

    void start(const QString &sessionFileName);
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();
    int getModuleConfigWriteCounts() const;

    VeinStorage::AbstractEventSystem *getVeinStorageSystem();
    VeinStorage::AbstractDatabase* getVeinStorageDb();
    VfCmdEventHandlerSystemPtr getVfCmdEventHandlerSystemPtr();
    void setVfComponent(int entityId, const QString &componentName, const QVariant &newValue);
    QVariant getVfComponent(int entityId, const QString &componentName);

    ModuleManagerSetupFacade* getModManFacade();
    ZeraModules::VirtualModule *getModule(const QString &uniqueName, int entityId);
    void setRangeGetSetDelay(int rangeGetSetDelay);

    ZDspServer *getDspServer();
    cSEC1000dServer *getSecServer();
    TestDspInterfacePtr getDspInterface(int entityId, DspInterfaceCreatedBy createdBy = MODULEPROG);
    TestDspInterfacePtr getDspInterface(DspInterfaceInjectableTypes injectType);
    QMap<int, QList<TestDspInterfacePtr>> getAllDspInterfaces();
    static void fireActualValues(ModuleManagerTestRunner* modmanTestRunner, const QString &session);

    // hotplug full functional methods
    void fireHotplugInterrupt(const AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap &deviceMap);
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap getCurrentHotplugMap() const;
    cSenseSettingsPtr getSenseSettings();

    // hotplug convenience methods
    void addStandardEmobControllers(const QStringList &channelAliases);
    void removeAllHotplugDevices();
    void addClamps(const QList<AbstractMockAllServices::clampParam> &clampParams);

private:
    void setupVfLogger();

    std::unique_ptr<TestLicenseSystem> m_licenseSystem;
    std::unique_ptr<ModuleManagerSetupFacade> m_modmanFacade;
    TestFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    std::unique_ptr<TestModuleManager> m_modMan;

    VfCmdEventHandlerSystemPtr m_vfCmdEventHandlerSystem;
    std::unique_ptr<TestDbAddSignaller> m_testSignaller;
    std::unique_ptr<VeinLogger::DatabaseLogger> m_dataLoggerSystem;
    bool m_dataLoggerSystemInitialized = false;
    bool m_cleanupLxdmFiles = false;
};

#endif // MODULEMANAGERTESTRUNNER_H
