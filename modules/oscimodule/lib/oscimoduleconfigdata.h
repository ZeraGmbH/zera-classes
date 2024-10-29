#ifndef OSCIMODULECONFIGDATA_H
#define OSCIMODULECONFIGDATA_H

#include <QString>
#include <QList>

#include "networkconnectioninfo.h"

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
    quint8 m_nValueCount; // how many measurment values
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    quint16 m_nInterpolation; // sample count we have to interpolate to
    quint16 m_nGap; // how many sample periods are left before sampling next
    stringParameter m_RefChannel;
};

}
#endif // OSCIMODULECONFIGDATA_H
