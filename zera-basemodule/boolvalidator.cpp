#include <QJsonArray>
#include <QJsonObject>

#include "boolvalidator.h"


bool cBoolValidator::isValidParam(QVariant newValue)
{
   bool ok;
   int onoff;

   onoff = newValue.toInt(&ok);
   return (ok && ((onoff == 0) || (onoff ==1))); // we only accept 0,1
}


void cBoolValidator::exportMetaData(QJsonObject& jsObj)
{
    jsObj.insert("Type", "BOOL");

    QJsonArray jsonArr = {0,1};
    jsObj.insert("Data", jsonArr);
}
