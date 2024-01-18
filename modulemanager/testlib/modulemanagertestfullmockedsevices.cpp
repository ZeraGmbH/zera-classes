#include "modulemanagertestfullmockedsevices.h"
#include "modulemanagerconfigtest.h"
#include "jsonsessionloadertest.h"
#include <timemachineobject.h>
#include <QDir>

void ModuleManagerTestFullMockedSevices::enableTest()
{
    m_runningInTest = true;
    JsonSessionLoaderTest::enableTests();
    ModulemanagerConfigTest::enableTest();
}

void ModuleManagerTestFullMockedSevices::pointToInstalledSessionFiles()
{
    m_sessionPath = QDir::cleanPath(
                    QString(OE_INSTALL_ROOT) + "/" +
                    QString(MODMAN_SESSION_PATH));
}

void ModuleManagerTestFullMockedSevices::pointToSourceSessionFiles()
{
    m_sessionPath = QDir::cleanPath(QString(SESSION_FILES_SOURCE_PATH));
}

bool ModuleManagerTestFullMockedSevices::loadTestSession(const QString sessionFileNameFull)
{
    return loadSession(sessionFileNameFull);
}

ModuleManagerTestFullMockedSevices::ModuleManagerTestFullMockedSevices(ModuleManagerSetupFacade *setupFacade, bool demo, QObject *parent) :
    ModuleManager(setupFacade, demo, parent)
{
}

void ModuleManagerTestFullMockedSevices::destroyModulesAndWaitUntilAllShutdown()
{
    destroyModules();
    waitUntilModulesAreReady();
}

void ModuleManagerTestFullMockedSevices::waitUntilModulesAreReady()
{
    do
        TimeMachineObject::feedEventLoop();
    while(!modulesReady());
}

QList<ZeraModules::ModuleData *> ModuleManagerTestFullMockedSevices::getModuleList()
{
    return m_moduleList;
}

QStringList ModuleManagerTestFullMockedSevices::getModuleFileNames()
{
    QString strBuildPath = MODULE_PLUGIN_BUILD_PATH;
    QStringList buildPaths = strBuildPath.split(" ", Qt::SkipEmptyParts);
    QStringList plugins;
    for(auto &buildPath : buildPaths) {
        QStringList libFiles = QDir(buildPath, "*.so").entryList();
        QString libFullPath = QDir::cleanPath(buildPath + "/" + libFiles[0]);
        plugins.append(libFullPath);
    }
    return plugins;
}
