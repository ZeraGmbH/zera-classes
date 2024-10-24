#ifndef TESTMODULEMANAGER_H
#define TESTMODULEMANAGER_H

#include <modulemanager.h>

class TestModuleManager : public ZeraModules::ModuleManager
{
public:
    static void enableTests();
    static void pointToInstalledSessionFiles();

    explicit TestModuleManager(ModuleManagerSetupFacade *setupFacade,
                               AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory);
    void startAllTestServices(QString deviceName, bool initialAdjPermission);
    void destroyModulesAndWaitUntilAllShutdown();
    void waitUntilModulesAreReady();
    QList<ZeraModules::ModuleData *> getModuleList();
    ZeraModules::VirtualModule *getModule(QString uniqueName, int entityId);

private:
    bool modulesReady();
    QStringList getModuleFileNames() override;
    // Avoid file write in install area on tests (OE tests turn 'ff')
    void saveModuleConfig(ZeraModules::ModuleData *) override {};
    void saveDefaultSession() override {};
};

#endif // TESTMODULEMANAGER_H
