#ifndef MODULEMANAGERTEST_H
#define MODULEMANAGERTEST_H

#include <modulemanager.h>

class ModuleManagerTest : public ZeraModules::ModuleManager
{
public:
    explicit ModuleManagerTest(const QStringList &sessionList, QObject *parent = nullptr);
    static void enableTest();
private:
    virtual QStringList getModuleFileNames() override;
};

#endif // MODULEMANAGERTEST_H
