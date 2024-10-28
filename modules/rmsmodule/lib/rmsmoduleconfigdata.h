#ifndef RMSMODULECONFIGDATA_H
#define RMSMODULECONFIGDATA_H

#include <QString>
#include <QList>

#include "socket.h"

namespace RMSMODULE
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


class cRmsModuleConfigData
{
public:
    cRmsModuleConfigData(){}
    quint8 m_nValueCount; // how many measurment values
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    QString m_sIntegrationMode; // we integrate over time or periods
    doubleParameter m_fMeasIntervalTime; // measuring interval 0.1 .. 100.0 sec.
    intParameter m_nMeasIntervalPeriod; // measuring periods 1 .. 10000
    double m_fmovingwindowInterval;
    bool m_bmovingWindow;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
};

}
#endif // RMSMODULECONFIGDATA_H
