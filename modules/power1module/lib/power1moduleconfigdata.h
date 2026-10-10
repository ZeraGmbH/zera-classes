#ifndef POWER1MODULECONFIGDATA_H
#define POWER1MODULECONFIGDATA_H

#include "configtypes.h"
#include <QStringList>

namespace POWER1MODULE
{

struct scaling
{
    int m_entityId = -1;
    QString m_componentName = "";
};

struct freqoutconfiguration
{
    QString m_sName;     // system name of the used frequency output
    int m_nSource = 0;   // from xml we get pms1,2,3,s and convert to 0 .. 3
    int m_nFoutMode = 0; // from xml we get +-,+,- we convert this to foutmodes
    QString m_sFreqOutNameDisplayed;
    scaling m_uscale;    // u actual value pre scaling provider
    scaling m_iscale;    // i actual value pre scaling provider
};


class cPower1ModuleConfigData
{
public:
    bool supportsVariableQrefFrequency() const {
        return m_sMeasmodeList.contains("QREF") &&
               hasQrefFrequency();
    }
    bool hasQrefFrequency() const { return !m_qrefFrequency.m_sKey.isEmpty(); }

    quint8 m_nMeasModeCount = 0;      // how many measurement modes do we support
    QStringList m_sMeasmodeList;      // a list of our measurement modes
    QStringList m_measmodePhaseList;  // a list comma separated measurement mode / phase e.g XLW,001
    QStringList m_sMeasSystemList;    // our measuring systems "m0,m1"
    QString m_sIntegrationMode;       // we integrate over time or periods
    intParameter m_nNominalFrequency; // our nominal frequency output for full range power
    intParameter m_nNominalFrequencyDefault;
    QString m_sFreqActualizationMode; // signalperiod or integrationtime
    quint8 m_nFreqOutputCount = 0;    // how many frequency ouptuts do we support
    QList<freqoutconfiguration> m_FreqOutputConfList; // a list of configuration values for each frequency output

    stringParameter m_sMeasuringMode;
    doubleParameter m_fMeasIntervalTime; // measuring interval 0.1 .. 100.0 sec.
    intParameter m_nMeasIntervalPeriod;  // measuring periods 1 .. 10000
    double m_fmovingwindowInterval = 0;
    bool m_disablephaseselect = false;
    bool m_bmovingWindow = false;
    bool m_enableScpiCommands = false;
    doubleParameter m_qrefFrequency;
};

}
#endif // POWER1MODULECONFIGDATA_H
