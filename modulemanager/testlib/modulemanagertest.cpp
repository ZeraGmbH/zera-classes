#include "modulemanagertest.h"
#include <QDir>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>

void ModuleManagerTest::enableTest()
{
    m_runningInTest = true;
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

ModuleManagerTest::ModuleManagerTest(const QStringList &sessionList, QObject *parent) :
    ModuleManager(sessionList, parent)
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
