#ifndef POWER1DSPVARGENERATOR_H
#define POWER1DSPVARGENERATOR_H

#include "power1moduleconfigdata.h"
#include "dspinterface.h"
#include "dspmeasdata.h"

class Power1DspVarGenerator
{
public:
    Power1DspVarGenerator();
    void setupVarList(Zera::cDSPInterface* pDSPInterFace, const POWER1MODULE::cPower1ModuleConfigData* confData, quint32 sampleRate);
    cDspMeasData* getActualValues();
    cDspMeasData* getFreqScale();
    cDspMeasData* getNominalPower();
    cDspMeasData* getParameters();
private:
    cDspMeasData* m_pTmpDataDsp = nullptr;
    cDspMeasData* m_pParameterDSP = nullptr;
    cDspMeasData* m_pActualValuesDSP = nullptr;
    cDspMeasData* m_pfreqScaleDSP = nullptr;
    cDspMeasData* m_pNomPower = nullptr;
};

#endif // POWER1DSPVARGENERATOR_H
