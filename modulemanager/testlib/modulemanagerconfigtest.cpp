#include "modulemanagerconfigtest.h"
#include <QDir>

void ModulemanagerConfigTest::enableTest()
{
    m_configFileDir = QDir::cleanPath(
        QString(OE_INSTALL_ROOT) + "/" + QString(MODMAN_CONFIG_PATH));
}

void ModulemanagerConfigTest::setConfigFile(QString configFile)
{
    m_configFileName = configFile;
}

void ModulemanagerConfigTest::setDeviceName(QString deviceName)
{
    m_deviceName = deviceName;
}
