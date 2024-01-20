#include "testmodulemanager.h"
#include "modulemanagerconfigtest.h"
#include "jsonsessionloadertest.h"
#include <timemachineobject.h>
#include <QDir>

void TestModuleManager::enableTest()
{
    m_runningInTest = true;
    JsonSessionLoaderTest::enableTests();
    ModulemanagerConfigTest::enableTest();
}

void TestModuleManager::pointToInstalledSessionFiles()
{
    m_sessionPath = QDir::cleanPath(
                    QString(OE_INSTALL_ROOT) + "/" +
                    QString(MODMAN_SESSION_PATH));
}

void TestModuleManager::pointToSourceSessionFiles()
{
    m_sessionPath = QDir::cleanPath(QString(SESSION_FILES_SOURCE_PATH));
}

bool TestModuleManager::loadTestSession(const QString sessionFileNameFull)
{
    return loadSession(sessionFileNameFull);
}

TestModuleManager::TestModuleManager(ModuleManagerSetupFacade *setupFacade, bool demo, QObject *parent) :
    ModuleManager(setupFacade, demo, parent)
{
}

void TestModuleManager::destroyModulesAndWaitUntilAllShutdown()
{
    destroyModules();
    waitUntilModulesAreReady();
}

void TestModuleManager::waitUntilModulesAreReady()
{
    do
        TimeMachineObject::feedEventLoop();
    while(!modulesReady());
}

QList<ZeraModules::ModuleData *> TestModuleManager::getModuleList()
{
    return m_moduleList;
}

QStringList TestModuleManager::getModuleFileNames()
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
