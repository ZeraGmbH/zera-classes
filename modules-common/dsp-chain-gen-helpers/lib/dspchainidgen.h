#ifndef DSPCHAINIDGEN_H
#define DSPCHAINIDGEN_H

#include <QtGlobal>

// Create unique ids for DSP chains automatically
// no overflow handling yet
class DspChainIdGen
{
public:
    DspChainIdGen(quint8 mainChain = 1);
    DspChainIdGen(const DspChainIdGen&) = delete;
    quint16 getNextChainId();
    QString getNextChainIdStr();
private:
    quint8 m_mainChain;
    quint8 m_subChain = 1;
};

#endif // DSPCHAINIDGEN_H
