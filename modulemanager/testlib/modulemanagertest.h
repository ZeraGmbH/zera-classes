#ifndef MODULEMANAGERTEST_H
#define MODULEMANAGERTEST_H

#include <modulemanager.h>

class ModuleManagerTest : public ZeraModules::ModuleManager
{
public:
    explicit ModuleManagerTest(ModuleManagerSetupFacade *setupFacade, bool demo = false, QObject *parent = nullptr);
    static void enableTest();
    static void pointToInstalledSessionFiles();
    static void pointToSourceSessionFiles();
    void changeMockServices(QString deviceName);
    void destroyModulesAndWaitUntilAllShutdown();
    QStringList getSessionFiles();
private:
    virtual QStringList getModuleFileNames() override;
};

#endif // MODULEMANAGERTEST_H
