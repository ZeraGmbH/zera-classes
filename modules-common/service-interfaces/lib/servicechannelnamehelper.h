#ifndef SERVICECHANNELNAMEHELPER_H
#define SERVICECHANNELNAMEHELPER_H

#include <QString>

class ServiceChannelNameHelper
{
public:
    static bool isPhaseNeutralVoltage(QString valueChannelName);
    static bool isCurrent(QString valueChannelName);
};

#endif // SERVICECHANNELNAMEHELPER_H
