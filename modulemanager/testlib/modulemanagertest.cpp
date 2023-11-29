#include "modulemanagertest.h"
#include "modulemanagerconfigtest.h"
#include "jsonsessionloadertest.h"
#include <QDir>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>

void ModuleManagerTest::enableTest()
{
    m_runningInTest = true;
    JsonSessionLoaderTest::enableTests();
    ModulemanagerConfigTest::enableTest();
}

void ModuleManagerTest::pointToInstalledSessionFiles()
{
    m_sessionPath = QDir::cleanPath(
                    QString(OE_INSTALL_ROOT) + "/" +
                    QString(MODMAN_SESSION_PATH));
}

void ModuleManagerTest::pointToSourceSessionFiles()
{
    m_sessionPath = QDir::cleanPath(QString(SESSION_FILES_SOURCE_PATH));
}

void ModuleManagerTest::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}

void ModuleManagerTest::changeMockServices(QString deviceName)
{
    setMockServices(deviceName);
}

ModuleManagerTest::ModuleManagerTest(const QStringList &sessionList, ModuleManagerSetupFacade *setupFacade, bool demo, QObject *parent) :
    ModuleManager(sessionList, setupFacade, demo, parent)
{
}

QStringList ModuleManagerTest::getModuleFileNames()
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
