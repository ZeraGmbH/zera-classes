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


class cRmsModuleConfigData
{
public:
    cRmsModuleConfigData(){}
    quint8 m_nDebugLevel;
    quint8 m_nValueCount; // how many measurment values
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    doubleParameter m_fMeasInterval; // measuring interval 0.1 .. 5.0 sec.
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
};

}
#endif // RMSMODULECONFIGDATA_H
