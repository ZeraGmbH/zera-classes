#include "servicechannelnamehelper.h"
#include <QStringList>

QStringList ServiceChannelNameHelper::getVoltageChannelNamesUsed(bool phaseSeqenceSwapped)
{
    if(phaseSeqenceSwapped)
        return QStringList() << "m0" << "m2" << "m1" << "m6";
    return QStringList() << "m0" << "m1" << "m2" << "m6";
}

QStringList ServiceChannelNameHelper::getCurrentChannelNamesUsed(bool phaseSeqenceSwapped)
{
    if(phaseSeqenceSwapped)
        return QStringList() << "m3" << "m5" << "m6" << "m7";
    return QStringList() << "m3" << "m4" << "m5" << "m7";
}

bool ServiceChannelNameHelper::isPhaseNeutralVoltage(QString valueChannelName)
{
    return getVoltageChannelNamesUsed().contains(valueChannelName);
}

bool ServiceChannelNameHelper::isCurrent(QString valueChannelName)
{
    return getCurrentChannelNamesUsed().contains(valueChannelName);
}
