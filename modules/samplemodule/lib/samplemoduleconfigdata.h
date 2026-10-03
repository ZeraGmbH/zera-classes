#ifndef SAMPLEMODULECONFIGDATA_H
#define SAMPLEMODULECONFIGDATA_H

#include "configtypes.h"
#include <QStringList>

namespace SAMPLEMODULE
{

struct cObsermaticConfPar
{
    bool m_bpllFixed = false;      // DC / no pll automatic
    boolParameter m_npllAutoAct;   // pll automatic active or not 1,0
    quint8 m_npllChannelCount = 0; // how many channels for pll setting
    QStringList m_pllChannelList;  // a list of channel system names the pll can be set to
    stringParameter m_pllSystemChannel;
};

class cSampleModuleConfigData
{
public:
    cSampleModuleConfigData(){}
    cObsermaticConfPar m_ObsermaticConfPar;
};

}

#endif // SAMPLEMODULECONFIGDATA_H
