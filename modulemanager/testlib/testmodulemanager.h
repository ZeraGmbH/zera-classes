#ifndef TESTMODULEMANAGER_H
#define TESTMODULEMANAGER_H

#include <modulemanager.h>

class TestModuleManager : public ZeraModules::ModuleManager
{
public:
    static void enableTest();
    static void pointToInstalledSessionFiles();
    static void pointToSourceSessionFiles();

    explicit TestModuleManager(ModuleManagerSetupFacade *setupFacade, bool demo = false, QObject *parent = nullptr);
    void setTestServices(QString deviceName);
    void destroyModulesAndWaitUntilAllShutdown();
    void waitUntilModulesAreReady();
    QList<ZeraModules::ModuleData *> getModuleList();
private:
    QStringList getModuleFileNames() override;
};

#endif // TESTMODULEMANAGER_H
