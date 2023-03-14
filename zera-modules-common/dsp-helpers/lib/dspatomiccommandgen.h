#ifndef DSPATOMICCOMMANDGEN_H
#define DSPATOMICCOMMANDGEN_H

#include <QString>

class DspAtomicCommandGen
{
public:
    static QString getCmdStartChainActive(quint16 dspChainId);
    static QString getStartChainInactive(quint16 dspChainId);
    static QString getStopChain(quint16 dspChainId);
    static QString getActivateChain(quint16 dspChainId);
    static QString getDeactivateChain(quint16 dspChainId);
};

#endif // DSPATOMICCOMMANDGEN_H
