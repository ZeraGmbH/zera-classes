#include "servicechannelnamehelper.h"
#include <QStringList>

bool ServiceChannelNameHelper::isPhaseNeutralVoltage(QString valueChannelName)
{
    QStringList voltagePhases = QStringList() << "m0" << "m1" << "m2" << "m6";
    return voltagePhases.contains(valueChannelName);
}

bool ServiceChannelNameHelper::isCurrent(QString valueChannelName)
{
    QStringList currentPhases = QStringList() << "m3" << "m4" << "m5" << "m7";
    return currentPhases.contains(valueChannelName);
}
