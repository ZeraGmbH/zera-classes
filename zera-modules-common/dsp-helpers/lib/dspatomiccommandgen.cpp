#include "dspatomiccommandgen.h"
#include "inttohexstringconvert.h"

QString DspAtomicCommandGen::getCmdStartChainActive(quint16 dspChainId)
{
    return QString("STARTCHAIN(1,1,%1)").arg(IntToHexStringConvert::convert(dspChainId)); // aktiv, prozessnr. (dummy),hauptkette 1 subkette X
}

QString DspAtomicCommandGen::getStartChainInactive(quint16 dspChainId)
{
    return QString("STARTCHAIN(0,1,%1)").arg(IntToHexStringConvert::convert(dspChainId)); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette X
}

QString DspAtomicCommandGen::getStopChain(quint16 dspChainId)
{
    return QString("STOPCHAIN(1,%1)").arg(IntToHexStringConvert::convert(dspChainId)); // ende prozessnr., hauptkette 1 subkette x
}

QString DspAtomicCommandGen::getActivateChain(quint16 dspChainId)
{
    return QString("ACTIVATECHAIN(1,%1)").arg(IntToHexStringConvert::convert(dspChainId));
}

QString DspAtomicCommandGen::getDeactivateChain(quint16 dspChainId)
{
    return QString("DEACTIVATECHAIN(1,%1)").arg(IntToHexStringConvert::convert(dspChainId));
}

