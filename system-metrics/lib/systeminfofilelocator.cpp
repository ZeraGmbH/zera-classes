#include "systeminfofilelocator.h"

QString SystemInfoFileLocator::m_procStatusFileName = QStringLiteral("/proc/stat");

QString SystemInfoFileLocator::getProcStatusFileName()
{
    return m_procStatusFileName;
}
