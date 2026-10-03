#ifndef FFTMODULECONFIGDATA_H
#define FFTMODULECONFIGDATA_H

#include "configtypes.h"
#include <QStringList>

namespace FFTMODULE
{

class cFftModuleConfigData
{
public:
    cFftModuleConfigData(){}
    quint8 m_nFftOrder = 0;
    quint8 m_nValueCount = 0;       // how many measurement values
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    stringParameter m_RefChannel;
    doubleParameter m_fMeasInterval; // measuring interval 0.1 .. 100.0 sec.
    double m_fmovingwindowInterval = 0.0;
    bool m_bmovingWindow = false;
};

}
#endif // FFTMODULECONFIGDATA_H
