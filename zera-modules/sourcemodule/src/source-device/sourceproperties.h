#ifndef SOURCEPROPERTIES_H
#define SOURCEPROPERTIES_H

#include "supportedsources.h"
#include <QString>

class SourceProperties
{
public:
    SourceProperties();
    SourceProperties(SupportedSourceTypes type, QString name, QString version);

    SupportedSourceTypes getType() const;
    QString getName() const;
    QString getVersion() const;
private:
    SupportedSourceTypes m_type;
    QString m_name;
    QString m_version;

};

#endif // SOURCEPROPERTIES_H
