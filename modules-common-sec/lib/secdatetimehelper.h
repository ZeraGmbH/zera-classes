#ifndef SECDATETIMEHELPER_H
#define SECDATETIMEHELPER_H

#include "vfmoduleparameter.h"
#include <QDateTime>

class SecDateTimeHelper
{
public:
    static void setDateTimeNow(QDateTime &var, VfModuleParameter* veinParam);
    static void setDateTime(const QDateTime &var, VfModuleParameter* veinParam);
};

#endif // SECDATETIMEHELPER_H
