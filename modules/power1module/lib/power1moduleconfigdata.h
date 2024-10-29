#ifndef POWER1MODULECONFIGDATA_H
#define POWER1MODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "networkconnectioninfo.h"
#include "foutinfo.h"

namespace POWER1MODULE
{

// used for configuration export
struct doubleParameter
{
    QString m_sKey;
    double m_fValue;
};


struct intParameter
{
    QString m_sKey;
    quint32 m_nValue;
};


struct stringParameter
{
    QString m_sKey;
    QString m_sValue;
};


struct scaling
{
    int m_entityId = -1;
    QString m_componentName = "";
};

struct freqoutconfiguration
{
    QString m_sName; // system name of the used frequency output
    int m_nSource; // from xml we get pms1,2,3,s and convert to 0 .. 3
    int m_nFoutMode; // from xml we get +-,+,- we convert this to foutmodes
    QString m_sFreqOutNameDisplayed;
    scaling m_uscale; // u actual value pre scaling provider
    scaling m_iscale; // i actual value pre scaling provider
};


class cPower1ModuleConfigData
{
public:
    cPower1ModuleConfigData(){}

    NetworkConnectionInfo m_RMSocket; // the sockets we can connect to
    NetworkConnectionInfo m_PCBServerSocket;
    NetworkConnectionInfo m_DSPServerSocket;

    quint8 m_nMeasModeCount; // how many measurement modes do we support
    QStringList m_sMeasmodeList; // a list of our measurement modes
    int m_measmodePhaseCount;
    QStringList m_measmodePhaseList; // a list comma separated measurement mode / phase e.g XLW,001
    QStringList m_sMeasSystemList; // our measuring systems "m0,m1"
    QString m_sIntegrationMode; // we integrate over time or periods
    quint32 m_nNominalFrequency; // our nominal frequency output for full range power
    QString m_sFreqActualizationMode; // signalperiod or integrationtime
    quint8 m_nFreqOutputCount; // how many frequency ouptuts do we support
    QList<freqoutconfiguration> m_FreqOutputConfList; // a list of configuration values for each frequency output

    stringParameter m_sMeasuringMode;
    stringParameter m_sXMeasModePhases;
    doubleParameter m_fMeasIntervalTime; // measuring interval 0.1 .. 100.0 sec.
    intParameter m_nMeasIntervalPeriod; // measuring periods 1 .. 10000
    double m_fmovingwindowInterval;
    bool m_disablephaseselect = false;
    bool m_bmovingWindow;
    bool m_enableScpiCommands;
};

}
#endif // POWER1MODULECONFIGDATA_H
