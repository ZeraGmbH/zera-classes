#ifndef POWER1MODULECONFIGDATA_H
#define POWER1MODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"

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


struct freqoutconfiguration
{
    QString m_sFreqOutput;
    QString m_sSource;
    QString m_sType;
};


class cPower1ModuleConfigData
{
public:
    cPower1ModuleConfigData(){}

    quint8 m_nDebugLevel;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;

    quint8 m_nMeasModeCount; // how many measurement modes do we support
    QStringList m_sMeasmodeList; // a list of our measurement modes
    QStringList m_sMeasSystemList; // our measuring systems "m0,m1" ....
    QString m_sM2WSystem; // the system on which 2wire measuring mode shall work on
    QString m_sIntegrationMode; // we integrate over time or periods
    quint32 m_nNominalFrequency; // our nominal frequency output for full range power
    QString m_sFreqActualizationMode; // signalperiod or integrationtime
    quint8 m_nFreqOutputCount; // how many frequency ouptuts do we support
    QList<freqoutconfiguration> m_FreqOutputConfList; // a list of configuration values for each frequency output

    stringParameter m_MeasuringMode;
    doubleParameter m_fMeasInterval; // measuring interval 0.1 .. 5.0 sec.
    intParameter m_nMeasPeriod; // measuring periods 1 .. 10000
};

}
#endif // POWER1MODULECONFIGDATA_H
