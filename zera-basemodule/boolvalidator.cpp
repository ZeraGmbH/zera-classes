#include <QJsonArray>
#include <QJsonObject>
#include <QMetaType>
#include "boolvalidator.h"


bool cBoolValidator::isValidParam(QVariant newValue)
{
   bool ok;
   int onoff;

   if (newValue.type() == QMetaType::Int)
   {
       onoff = newValue.toInt(&ok);
       return (ok && ((onoff == 0) || (onoff ==1))); // we only accept 0,1
   }
   else
       return false;
}


void cBoolValidator::exportMetaData(QJsonObject& jsObj)
{
    jsObj.insert("Type", "BOOL");

    QJsonArray jsonArr = {0,1};
    jsObj.insert("Data", jsonArr);
}
