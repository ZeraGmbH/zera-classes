#ifndef MODULEMANAGERTESTFULLMOCKEDSEVICES_H
#define MODULEMANAGERTESTFULLMOCKEDSEVICES_H

#include <modulemanager.h>

class ModuleManagerTestFullMockedSevices : public ZeraModules::ModuleManager
{
public:
    explicit ModuleManagerTestFullMockedSevices(ModuleManagerSetupFacade *setupFacade, bool demo = false, QObject *parent = nullptr);
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

#endif // MODULEMANAGERTESTFULLMOCKEDSEVICES_H
