#ifndef POWER1DSPVARGENERATOR_H
#define POWER1DSPVARGENERATOR_H

#include "power1moduleconfigdata.h"
#include "dspinterface.h"
#include "dspvargroupclientinterface.h"

class Power1DspVarGenerator
{
public:
    void setupVarList(Zera::cDSPInterface* pDSPInterFace, const POWER1MODULE::cPower1ModuleConfigData* confData, quint32 sampleRate);
    DspVarGroupClientInterface* getActualValues();
    DspVarGroupClientInterface* getFreqScale();
    DspVarGroupClientInterface* getNominalPower();
    DspVarGroupClientInterface* getParameters();
private:
    DspVarGroupClientInterface* m_pParameterDSP = nullptr;
    DspVarGroupClientInterface* m_pActualValuesDSP = nullptr;
    DspVarGroupClientInterface* m_pfreqScaleDSP = nullptr;
    DspVarGroupClientInterface* m_pNomPower = nullptr;
};

#endif // POWER1DSPVARGENERATOR_H
