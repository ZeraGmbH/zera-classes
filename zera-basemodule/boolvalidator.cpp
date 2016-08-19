#include <QJsonArray>
#include <QJsonObject>
#include <QMetaType>
#include "boolvalidator.h"


bool cBoolValidator::isValidParam(QVariant& newValue)
{
   bool ok;
   int onoff;

   onoff = newValue.toInt(&ok);

   if (ok && ((onoff == 0) || (onoff ==1)))
   {
       QString s;
       s = newValue.toString();
       if ( (s == "true") or (s == "false") )
               newValue = onoff;
       return true;
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
