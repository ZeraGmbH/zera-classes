#include "secdatetimehelper.h"

void SecDateTimeHelper::setDateTimeNow(QDateTime &var, VfModuleParameter *veinParam)
{
    var = QDateTime::currentDateTime();
    setDateTime(var, veinParam);
}

void SecDateTimeHelper::setDateTime(const QDateTime &var, VfModuleParameter *veinParam)
{
    veinParam->setValue(var.toString("dd-MM-yyyy HH:mm:ss"));
}
