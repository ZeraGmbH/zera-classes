#ifndef DSPCHAINIDGEN_H
#define DSPCHAINIDGEN_H

#include <QtGlobal>

// no overflow handling yet
class DspChainIdGen
{
public:
    DspChainIdGen(quint8 mainChain = 1);
    QString getNextChainId();
private:
    quint8 m_mainChain;
    quint8 m_subChain = 1;
};

#endif // DSPCHAINIDGEN_H
