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
    QString m_sSampleSystem; // the sample system name (we only handle 1)
    bool m_bpllAuto; // pll automatic or not
    bool m_bpllFixed;
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
    double m_fMeasInterval; // measuring interval 0.1 .. 5.0 sec.
};

}

#endif // SAMPLEMODULECONFIGDATA_H
