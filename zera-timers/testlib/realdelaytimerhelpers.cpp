#include "realdelaytimerhelpers.h"
#include <QtGlobal>

bool RealDelayTimerHelpers::isExpireTimeWithinLimits(int measuredTime, int expectedTime)
{
    int minAllowed = expectedTime;
    int maxAllowed = expectedTime * 1.5;
    bool ok = measuredTime>=minAllowed && measuredTime<=maxAllowed;
    if(!ok) {
        qWarning("Measured: %i / Expected: %i", measuredTime, expectedTime);
        qWarning("Min allowed: %i / Max allowed: %i", minAllowed, maxAllowed);
    }
    return ok;
}
