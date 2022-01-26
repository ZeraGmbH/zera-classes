#ifndef SOURCEPROPERTIES_H
#define SOURCEPROPERTIES_H

#include "supportedsources.h"
#include <QString>

class SourceProperties
{
public:
    SourceProperties();
    SourceProperties(SupportedSourceTypes type, QString name, QString version);

    bool isValid() const;
    SupportedSourceTypes getType() const;
    QString getName() const;
    QString getVersion() const;
private:
    bool m_isValid = false;
    SupportedSourceTypes m_type = SOURCE_MT_COMMON;
    QString m_name;
    QString m_version;

};

#endif // SOURCEPROPERTIES_H
