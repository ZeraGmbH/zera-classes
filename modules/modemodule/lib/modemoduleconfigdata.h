#ifndef MODEMODULECONFIGDATA_H
#define MODEMODULECONFIGDATA_H

#include <QString>

#include "networkconnectioninfo.h"

namespace MODEMODULE
{

class cModeModuleConfigData
{
public:
    cModeModuleConfigData(){}
    QString m_sMode;
    NetworkConnectionInfo m_RMSocket; // the sockets we can connect to
    NetworkConnectionInfo m_PCBServerSocket;
    NetworkConnectionInfo m_DSPServerSocket;
    quint8 m_nChannelnr; // dsp sampling system channel nr
    quint16 m_nSignalPeriod; // dsp sampling system samples per signal period
    quint16 m_nMeasurePeriod; // dsp sampling system samples per measuring period
};

}

#endif // MODEMODULECONFIGDATA_H
