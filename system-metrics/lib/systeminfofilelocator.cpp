#include "systeminfofilelocator.h"

QString SystemInfoFileLocator::m_procStatusFileName = QStringLiteral("/proc/stat");
QString SystemInfoFileLocator::m_procMeminfoFileName = QStringLiteral("/proc/meminfo");

QString SystemInfoFileLocator::getProcStatusFileName()
{
    return m_procStatusFileName;
}

QString SystemInfoFileLocator::getProcMeminfoFileName()
{
    return m_procMeminfoFileName;
}
