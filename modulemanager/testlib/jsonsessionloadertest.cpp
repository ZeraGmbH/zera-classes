#include "jsonsessionloadertest.h"
#include <QDir>

void JsonSessionLoaderTest::supportOeTests()
{
    m_configDirName = QDir::cleanPath(
        QString(OE_INSTALL_ROOT) + "/" + QString(MODMAN_CONFIG_PATH));
}
