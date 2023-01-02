#ifndef STRINGVALIDATOR_H
#define STRINGVALIDATOR_H

#include "validatorinterface.h"
#include <QStringList>

class cStringValidator: public ValidatorInterface
{
public:
    cStringValidator(QString possibilities); // all possible strings with ';' delimiter
    cStringValidator(QStringList possibilities); // same but as qstringlist
    virtual bool isValidParam(QVariant &newValue);
    virtual void exportMetaData(QJsonObject& jsObj);
    virtual void setValidator(QString possibilities);
    virtual void setValidator(QStringList possibilities);
private:
    QStringList m_sPossibilitiesList;
};

#endif // STRINGVALIDATOR_H
