#ifndef SOURCEPROPERTIES_H
#define SOURCEPROPERTIES_H

#include "supportedsources.h"
#include "sourceprotocols.h"
#include <QString>

class SourceProperties
{
public:
    SourceProperties(); // unset
    enum eLocation {
        EXTERNAL,
        BUILTIN
    };
    SourceProperties(SupportedSourceTypes type,
                     QString name,
                     QString version,
                     SourceProtocols protocol,
                     eLocation location);

    bool wasSet() const;
    SupportedSourceTypes getType() const;
    QString getName() const;
    QString getVersion() const;
    SourceProtocols getProtocol() const;
    eLocation getLocation() const;

private:
    bool m_wasSet = false;
    SupportedSourceTypes m_type = SOURCE_MT_COMMON;
    QString m_name;
    QString m_version;
    SourceProtocols m_protocol;
    eLocation m_location = EXTERNAL;
};

#endif // SOURCEPROPERTIES_H
