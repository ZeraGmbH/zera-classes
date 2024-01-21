#include "testmodulemanager.h"
#include "testallservicescom5003.h"
#include "testallservicesmt310s2.h"
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

void TestModuleManager::setTestServices(QString deviceName)
{
    if (m_mockAllServices)
        m_mockAllServices = nullptr;

    if(deviceName == "mt310s2")
        m_mockAllServices = std::make_unique<TestAllServicesMt310s2>();
    else if(deviceName == "com5003")
        m_mockAllServices = std::make_unique<TestAllServicesCom5003>();
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
