#include "dspchainidgen.h"
#include "inttohexstringconvert.h"

DspChainIdGen::DspChainIdGen(quint8 mainChain) :
    m_mainChain(mainChain)
{
}

QString DspChainIdGen::getNextChainId()
{
    quint32 value = (m_mainChain<<8) + m_subChain;
    m_subChain++;
    return IntToHexStringConvert::convert(value);
}
