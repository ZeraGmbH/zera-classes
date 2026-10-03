#ifndef PERIODAVERAGEMODULECONFIGDATA_H
#define PERIODAVERAGEMODULECONFIGDATA_H

#include "configtypes.h"
#include <QList>

namespace PERIODAVERAGEMODULE
{

class PeriodAverageModuleConfigData
{
public:
    quint8 m_maxPeriods = 0;
    quint8 m_channelCount = 0;
    QStringList m_valueChannelList;
    intParameter m_periodCount;
};

}
#endif // PERIODAVERAGEMODULECONFIGDATA_H
