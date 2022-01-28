#include "sourceproperties.h"

SourceProperties::SourceProperties()
{
}

SourceProperties::SourceProperties(SupportedSourceTypes type, QString name, QString version, SourceProtocols protocol) :
    m_isValid(true),
    m_type(type),
    m_name(name),
    m_version(version),
    m_protocol(protocol)
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

SourceProtocols SourceProperties::getProtocol() const
{
    return m_protocol;
}

