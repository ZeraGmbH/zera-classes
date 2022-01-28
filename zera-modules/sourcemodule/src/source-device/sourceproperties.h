#ifndef SOURCEPROPERTIES_H
#define SOURCEPROPERTIES_H

#include "supportedsources.h"
#include "source-protocols/sourceprotocols.h"
#include <QString>

class SourceProperties
{
public:
    SourceProperties();
    SourceProperties(SupportedSourceTypes type,
                     QString name,
                     QString version,
                     SourceProtocols protocol = SourceProtocols::ZERA_SERIAL);

    bool isValid() const;
    SupportedSourceTypes getType() const;
    QString getName() const;
    QString getVersion() const;
    SourceProtocols getProtocol() const;
private:
    bool m_isValid = false;
    SupportedSourceTypes m_type = SOURCE_MT_COMMON;
    QString m_name;
    QString m_version;
    SourceProtocols m_protocol;
};

#endif // SOURCEPROPERTIES_H
