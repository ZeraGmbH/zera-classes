#ifndef MODULEMANAGERTEST_H
#define MODULEMANAGERTEST_H

#include <modulemanager.h>

class ModuleManagerTest : public ZeraModules::ModuleManager
{
public:
    explicit ModuleManagerTest(const QStringList &sessionList, ModuleManagerSetupFacade *setupFacade, bool demo = false, QObject *parent = nullptr);
    static void enableTest();
    static void pointToInstalledSessionFiles();
    static void pointToSourceSessionFiles();
    static void feedEventLoop();
    void changeMockServices(QString deviceName);
private:
    virtual QStringList getModuleFileNames() override;
};

#endif // MODULEMANAGERTEST_H
