#ifndef POWER1DSPVARGENERATOR_H
#define POWER1DSPVARGENERATOR_H

#include "power1moduleconfigdata.h"
#include "dspinterface.h"
#include "dspmemorygroups.h"

class Power1DspVarGenerator
{
public:
    Power1DspVarGenerator();
    void setupVarList(Zera::cDSPInterface* pDSPInterFace, const POWER1MODULE::cPower1ModuleConfigData* confData, quint32 sampleRate);
    void deleteVarList(Zera::cDSPInterface* pDSPInterFace);
    DspMemoryGroups* getActualValues();
    DspMemoryGroups* getFreqScale();
    DspMemoryGroups* getNominalPower();
    DspMemoryGroups* getParameters();
    quint32 getMemUsed();
private:
    DspMemoryGroups* m_pTmpDataDsp = nullptr;
    DspMemoryGroups* m_pParameterDSP = nullptr;
    DspMemoryGroups* m_pActualValuesDSP = nullptr;
    DspMemoryGroups* m_pfreqScaleDSP = nullptr;
    DspMemoryGroups* m_pNomPower = nullptr;
    quint32 m_nDspMemUsed = 0;
};

#endif // POWER1DSPVARGENERATOR_H
