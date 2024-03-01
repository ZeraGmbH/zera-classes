#ifndef SERVICECHANNELNAMEHELPER_H
#define SERVICECHANNELNAMEHELPER_H

#include <QString>

class ServiceChannelNameHelper
{
public:
    static QStringList getVoltageChannelNamesUsed(bool phaseSeqenceSwapped = false);
    static QStringList getCurrentChannelNamesUsed(bool phaseSeqenceSwapped = false);
    static bool isPhaseNeutralVoltage(QString valueChannelName);
    static bool isCurrent(QString valueChannelName);
};

#endif // SERVICECHANNELNAMEHELPER_H
