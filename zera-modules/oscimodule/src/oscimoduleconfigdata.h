#ifndef OSCIMODULECONFIGDATA_H
#define OSCIMODULECONFIGDATA_H

#include <QString>
#include <QList>

#include "socket.h"

namespace OSCIMODULE
{

struct stringParameter
{
    QString m_sKey;
    QString m_sPar;
};

class cOsciModuleConfigData
{
public:
    cOsciModuleConfigData(){}
    quint8 m_nDebugLevel;
    quint8 m_nValueCount; // how many measurment values
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    quint16 m_nInterpolation; // sample count we have to interpolate to
    quint16 m_nGap; // how many sample periods are left before sampling next
    stringParameter m_RefChannel;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
};

}
#endif // OSCIMODULECONFIGDATA_H
