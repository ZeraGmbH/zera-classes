#ifndef TESTMODULEMANAGER_H
#define TESTMODULEMANAGER_H

#include <modulemanager.h>

class TestModuleManager : public ZeraModules::ModuleManager
{
public:
    static void supportOeTests();
    static void pointToInstalledSessionFiles();

    explicit TestModuleManager(ModuleManagerSetupFacade *setupFacade, bool demo = false, QObject *parent = nullptr);
    void startAllServiceMocks(QString deviceName) override;
    void destroyModulesAndWaitUntilAllShutdown();
    void waitUntilModulesAreReady();
    QList<ZeraModules::ModuleData *> getModuleList();

private:
    bool modulesReady();
    QStringList getModuleFileNames() override;
    // Avoid file write in install area on tests (OE tests turn 'ff')
    void saveModuleConfig(ZeraModules::ModuleData *) override {};
    void saveDefaultSession() override {};
};

#endif // TESTMODULEMANAGER_H
