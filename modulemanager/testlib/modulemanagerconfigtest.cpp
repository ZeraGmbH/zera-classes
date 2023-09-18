#include "modulemanagerconfigtest.h"
#include <QDir>

void ModulemanagerConfigTest::enableTest()
{
    m_configFileName = QDir::cleanPath(QString(MODMAN_CONFIG_PATH) + "/" + "modulemanager_config_dev.json");
}
