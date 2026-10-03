#ifndef RMSMODULECONFIGDATA_H
#define RMSMODULECONFIGDATA_H

#include "configtypes.h"
#include <QStringList>

namespace RMSMODULE
{

class cRmsModuleConfigData
{
public:
    quint8 m_nValueCount = 0;            // how many measurment values
    QStringList m_valueChannelList;      // a list of channel or channel pairs we work on to generate our measurement values
    QString m_sIntegrationMode;          // we integrate over time or periods
    doubleParameter m_fMeasIntervalTime; // measuring interval 0.1 .. 100.0 sec.
    intParameter m_nMeasIntervalPeriod;  // measuring periods 1 .. 10000
    double m_fmovingwindowInterval = 0;
    bool m_bmovingWindow = false;
};

}
#endif // RMSMODULECONFIGDATA_H
