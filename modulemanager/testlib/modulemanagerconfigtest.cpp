#include "modulemanagerconfigtest.h"
#include <QDir>

void ModulemanagerConfigTest::enableTest()
{
    m_configFileDir = QDir::cleanPath(
        QString(OE_INSTALL_ROOT) + "/" + QString(MODMAN_CONFIG_PATH));
}
