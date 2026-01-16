#ifndef SAMPLEMODULECONFIGDATA_H
#define SAMPLEMODULECONFIGDATA_H

#include <QStringList>

namespace SAMPLEMODULE
{

// used for configuration export

struct boolParameter
{
    QString m_sKey;
    quint8 m_nActive; // active or not 1,0
};


struct stringParameter
{
    QString m_sKey;
    QString m_sPar;
};

struct cObsermaticConfPar
{
    bool m_bpllFixed;            // DC / no pll automatic
    boolParameter m_npllAutoAct; // pll automatic active or not 1,0
    quint8 m_npllChannelCount; // how many channels for pll setting
    QStringList m_pllChannelList; // a list of channel system names the pll can be set to
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
