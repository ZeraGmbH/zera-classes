#include "moduleconfigurationnull.h"

ModuleConfigurationNull::ModuleConfigurationNull()
{
    m_bConfigured = true;
}

QByteArray ModuleConfigurationNull::exportConfiguration() const
{
    return QByteArray();
}

void ModuleConfigurationNull::configXMLInfo(const QString &key)
{
    Q_UNUSED(key)
}
