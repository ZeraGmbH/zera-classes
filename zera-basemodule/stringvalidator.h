#ifndef STRINGVALIDATOR_H
#define STRINGVALIDATOR_H

#include <QStringList>

#include "paramvalidator.h"


class cStringValidator: public cParamValidator
{
public:
    cStringValidator(QString possibilities); // all possible strings with ';' delimiter
    cStringValidator(QStringList possibilities); // same but as qstringlist
    virtual bool isValidParam(QVariant &newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QStringList m_sPossibilitiesList;
};

#endif // STRINGVALIDATOR_H
