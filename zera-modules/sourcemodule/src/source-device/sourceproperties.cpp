#include "sourceproperties.h"

SourceProperties::SourceProperties()
{
}

SourceProperties::SourceProperties(SupportedSourceTypes type, QString name, QString version) :
    m_isValid(true),
    m_type(type),
    m_name(name),
    m_version(version)
{
}

bool SourceProperties::isValid() const
{
    return m_isValid;
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

