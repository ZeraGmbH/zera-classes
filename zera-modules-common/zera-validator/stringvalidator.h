#ifndef STRINGVALIDATOR_H
#define STRINGVALIDATOR_H

#include "validatorinterface.h"
#include <QStringList>

class cStringValidator: public ValidatorInterface
{
public:
    cStringValidator(QString possibilities); // all possible strings with ';' delimiter
    cStringValidator(QStringList possibilities); // same but as qstringlist
    bool isValidParam(QVariant &newValue) override;
    void exportMetaData(QJsonObject& jsObj) override;
    void setValidator(QString possibilities);
    void setValidator(QStringList possibilities);
private:
    QStringList m_sPossibilitiesList;
};

#endif // STRINGVALIDATOR_H
