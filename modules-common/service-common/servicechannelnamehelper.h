#ifndef SERVICECHANNELNAMEHELPER_H
#define SERVICECHANNELNAMEHELPER_H

#include "cro_systemobserver.h"
#include <QString>

class ServiceChannelNameHelper
{
public:
    static QStringList getVoltageChannelNamesUsed(bool phaseSeqenceSwapped = false);
    static QStringList getCurrentChannelNamesUsed(bool phaseSeqenceSwapped = false);
    static bool isPhaseNeutralVoltage(QString valueChannelName);
    static bool isCurrent(QString valueChannelName);

    struct TChannelAliasUnit
    {
        QString m_channelAlias;
        QString m_channelUnit;
    };
    static TChannelAliasUnit getChannelAndUnit(const QString &channelOrChannelPair,
                                               ChannelRangeObserver::SystemObserverPtr observer);
};

#endif // SERVICECHANNELNAMEHELPER_H
