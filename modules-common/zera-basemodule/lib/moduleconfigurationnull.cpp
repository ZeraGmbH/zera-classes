#include "moduleconfigurationnull.h"

ModuleConfigurationNull::ModuleConfigurationNull()
{
}

void ModuleConfigurationNull::setConfiguration(const QByteArray& xmlString)
{
    Q_UNUSED(xmlString)
    m_bConfigured = true;
    m_bConfigError = false;
}

QByteArray ModuleConfigurationNull::exportConfiguration()
{
    return QByteArray();
}

void ModuleConfigurationNull::configXMLInfo(const QString &key)
{
    Q_UNUSED(key)
}
