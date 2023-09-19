#include "modulemanagerconfigtest.h"
#include <QDir>

void ModulemanagerConfigTest::enableTest()
{
    setDemoDevice("demo");
    m_configFileDir = QDir::cleanPath(
        QString(OE_INSTALL_ROOT) + "/" + QString(MODMAN_CONFIG_PATH));
}
