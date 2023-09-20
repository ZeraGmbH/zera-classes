#include "modulemanagertest.h"
#include <QDir>

void ModuleManagerTest::enableTest()
{
    m_sessionPath = QDir::cleanPath(
                QString(OE_INSTALL_ROOT) + "/" +
                QString(MODMAN_SESSION_PATH));
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
