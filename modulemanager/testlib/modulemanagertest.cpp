#include "modulemanagertest.h"
#include <QDir>

void ModuleManagerTest::enableTest()
{
    m_sessionPath = QDir::cleanPath(
                QString(OE_INSTALL_ROOT) + "/" +
                QString(MODMAN_SESSION_PATH));
}
