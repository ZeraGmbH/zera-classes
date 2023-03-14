#ifndef POWER2MODULECONFIGDATA_H
#define POWER2MODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"
#include "foutinfo.h"

namespace POWER2MODULE
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


struct freqoutconfiguration
{
    QString m_sName; // system name of the used frequency output
    int m_nSource; // from xml we get pms1,2,3,s and convert to 0 .. 3
    int m_nFoutMode; // from xml we get +-,+,- we convert this to foutmodes
};


class cPower2ModuleConfigData
{
public:
    cPower2ModuleConfigData(){}

    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;

    quint8 m_nMeasModeCount; // how many measurement modes do we support
    QStringList m_sMeasmodeList; // a list of our measurement modes
    QStringList m_sMeasSystemList; // our measuring systems "m0,m1"
    QString m_sIntegrationMode; // we integrate over time or periods
    quint32 m_nNominalFrequency; // our nominal frequency output for full range power
    QString m_sFreqActualizationMode; // signalperiod or integrationtime
    quint8 m_nFreqOutputCount; // how many frequency ouptuts do we support
    QList<freqoutconfiguration> m_FreqOutputConfList; // a list of configuration values for each frequency output

    stringParameter m_sMeasuringMode;
    doubleParameter m_fMeasIntervalTime; // measuring interval 0.1 .. 100.0 sec.
    intParameter m_nMeasIntervalPeriod; // measuring periods 1 .. 10000
    double m_fmovingwindowInterval;
    bool m_bmovingWindow;
};

}
#endif // POWER2MODULECONFIGDATA_H
