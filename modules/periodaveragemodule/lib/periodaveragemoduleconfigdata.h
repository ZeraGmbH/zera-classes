#ifndef PERIODAVERAGEMODULECONFIGDATA_H
#define PERIODAVERAGEMODULECONFIGDATA_H

#include <QString>
#include <QList>

namespace PERIODAVERAGEMODULE
{

struct intParameter
{
    QString m_sKey;
    quint32 m_nValue;
};

class PeriodAverageModuleConfigData
{
public:
    quint8 m_maxPeriods;
    quint8 m_channelCount;
    QStringList m_valueChannelList;
    intParameter m_periodCount;
};

}
#endif // PERIODAVERAGEMODULECONFIGDATA_H
