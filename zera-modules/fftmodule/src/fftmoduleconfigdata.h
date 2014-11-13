#ifndef FFTMODULECONFIGDATA_H
#define FFTMODULECONFIGDATA_H

#include <QString>
#include <QList>

#include "socket.h"

namespace FFTMODULE
{

// used for configuration export
struct doubleParameter
{
    QString m_sKey;
    double m_fValue;
};


class cFftModuleConfigData
{
public:
    cFftModuleConfigData(){}
    quint8 m_nDebugLevel;
    quint8 m_nFftOrder;
    quint8 m_nValueCount; // how many measurement values
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    doubleParameter m_fMeasInterval; // measuring interval 0.1 .. 100.0 sec.
    double m_fmovingwindowInterval;
    bool m_bmovingWindow;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
};

}
#endif // FFTMODULECONFIGDATA_H
