#include "sourceproperties.h"

SourceProperties::SourceProperties()
{
}

SourceProperties::SourceProperties(SupportedSourceTypes type, QString name, QString version) :
    m_type(type),
    m_name(name),
    m_version(version)
{
}

SupportedSourceTypes SourceProperties::getType() const
{
    return m_type;
}

QString SourceProperties::getName() const
{
    return m_name;
}

QString SourceProperties::getVersion() const
{
    return m_version;
}

