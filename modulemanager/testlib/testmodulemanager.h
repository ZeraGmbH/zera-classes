#ifndef TESTMODULEMANAGER_H
#define TESTMODULEMANAGER_H

#include <modulemanager.h>

class TestModuleManager : public ZeraModules::ModuleManager
{
public:
    explicit TestModuleManager(ModuleManagerSetupFacade *setupFacade, bool demo = false, QObject *parent = nullptr);
    static void enableTest();
    static void pointToInstalledSessionFiles();
    static void pointToSourceSessionFiles();
    bool loadTestSession(const QString sessionFileNameFull);
    void destroyModulesAndWaitUntilAllShutdown();
    void waitUntilModulesAreReady();
    QList<ZeraModules::ModuleData *> getModuleList();
private:
    virtual QStringList getModuleFileNames() override;
};

#endif // TESTMODULEMANAGER_H
