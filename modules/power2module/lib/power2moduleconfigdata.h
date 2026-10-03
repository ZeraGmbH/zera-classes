#ifndef POWER2MODULECONFIGDATA_H
#define POWER2MODULECONFIGDATA_H

#include "configtypes.h"
#include <QStringList>
#include <QList>

namespace POWER2MODULE
{

struct freqoutconfiguration
{
    QString m_sName;     // system name of the used frequency output
    int m_nSource = 0;   // from xml we get pms1,2,3,s and convert to 0 .. 3
    int m_nFoutMode = 0; // from xml we get +-,+,- we convert this to foutmodes
};

class cPower2ModuleConfigData
{
public:
    quint8 m_nMeasModeCount = 0;      // how many measurement modes do we support
    QStringList m_sMeasmodeList;      // a list of our measurement modes
    QStringList m_sMeasSystemList;    // our measuring systems "m0,m1"
    QString m_sIntegrationMode;       // we integrate over time or periods
    quint32 m_nNominalFrequency = 0;  // our nominal frequency output for full range power
    QString m_sFreqActualizationMode; // signalperiod or integrationtime
    quint8 m_nFreqOutputCount = 0;    // how many frequency ouptuts do we support
    QList<freqoutconfiguration> m_FreqOutputConfList; // a list of configuration values for each frequency output

    stringParameter m_sMeasuringMode;
    doubleParameter m_fMeasIntervalTime; // measuring interval 0.1 .. 100.0 sec.
    intParameter m_nMeasIntervalPeriod; // measuring periods 1 .. 10000
    double m_fmovingwindowInterval = 0.0;
    bool m_bmovingWindow = false;
};

}
#endif // POWER2MODULECONFIGDATA_H
