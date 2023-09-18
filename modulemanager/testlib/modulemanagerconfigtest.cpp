#include "modulemanagerconfigtest.h"
#include <QDir>

void ModulemanagerConfigTest::enableTest()
{
    m_configFileName = QDir::cleanPath(
        QString(OE_INSTALL_ROOT) + "/" +
        QString(MODMAN_CONFIG_PATH) + "/" + "modulemanager_config_dev.json");
}
