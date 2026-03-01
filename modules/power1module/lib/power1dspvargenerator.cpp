#include "power1dspvargenerator.h"
#include "measmodeinfo.h"

void Power1DspVarGenerator::setupVarList(Zera::cDSPInterface *pDSPInterFace, const POWER1MODULE::cPower1ModuleConfigData *confData, quint32 sampleRate)
{
    // work variables without I/O
    DspVarGroupClientInterface* tmpDspVarGroup = pDSPInterFace->createVariableGroup("TmpData");
    tmpDspVarGroup->addDspVar("MEASSIGNAL1", sampleRate, DSPDATA::vDspTemp); // we need 2 signals for our computations
    tmpDspVarGroup->addDspVar("MEASSIGNAL2", sampleRate, DSPDATA::vDspTemp);
    tmpDspVarGroup->addDspVar("VALPQS", MeasPhaseCount+SumValueCount, DSPDATA::vDspTemp); // here x1, x2, x3 , xs will land
    tmpDspVarGroup->addDspVar("VAL_APPARENT_P", MeasPhaseCount, DSPDATA::vDspTemp);
    tmpDspVarGroup->addDspVar("VAL_APPARENT_Q", MeasPhaseCount, DSPDATA::vDspTemp);
    tmpDspVarGroup->addDspVar("TEMP1", 2, DSPDATA::vDspTemp); // we need 2 temp. vars also for complex
    tmpDspVarGroup->addDspVar("TEMP2", 2, DSPDATA::vDspTemp);
    tmpDspVarGroup->addDspVar("CONST_HALF", 1, DSPDATA::vDspTemp);
    tmpDspVarGroup->addDspVar("CONST_1_DIV_SQRT3", 1, DSPDATA::vDspTemp);
    tmpDspVarGroup->addDspVar("CONST_1_5", 1, DSPDATA::vDspTemp);
    tmpDspVarGroup->addDspVar("FILTER", DspBuffLen::avgFilterLen(MeasPhaseCount+SumValueCount), DSPDATA::vDspTemp);

    // a handle for parameter
    m_pParameterDSP =  pDSPInterFace->createVariableGroup("Parameter");
    m_pParameterDSP->addDspVar("TIPAR",1, DSPDATA::vDspParam, dspDataTypeInt); // integrationtime res = 1ms or period
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addDspVar("TISTART",1, DSPDATA::vDspParam, dspDataTypeInt);
    m_pParameterDSP->addDspVar("MMODE",1, DSPDATA::vDspParam, dspDataTypeInt);
    m_pParameterDSP->addDspVar("MMODE_SUM",1, DSPDATA::vDspParam, dspDataTypeInt);

    // for meas modes with phase wise enable disable
    for(int phase=0; phase<MeasPhaseCount; phase++) {
        QString varName = QString("XMMODEPHASE%1").arg(phase);
        m_pParameterDSP->addDspVar(varName, 1, DSPDATA::vDspParam, dspDataTypeInt);
    }

    // a handle for filtered actual values
    m_pActualValuesDSP = pDSPInterFace->createVariableGroup("ActualValues");
    m_pActualValuesDSP->addDspVar("VALPQSF", MeasPhaseCount+SumValueCount, DSPDATA::vDspResult);

    // and one for the frequency output scale values, we need 1 value for each configured output
    m_pfreqScaleDSP = pDSPInterFace->createVariableGroup("FrequencyScale");
    m_pfreqScaleDSP->addDspVar("FREQSCALE", confData->m_nFreqOutputCount, DSPDATA::vDspParam);

    // and one for nominal power in case that measuring mode is qref
    m_pNomPower = pDSPInterFace->createVariableGroup("QRefScale");
    m_pNomPower->addDspVar("NOMPOWER", 1, DSPDATA::vDspParam);
}

DspVarGroupClientInterface *Power1DspVarGenerator::getActualValues()
{
    return m_pActualValuesDSP;
}

DspVarGroupClientInterface *Power1DspVarGenerator::getFreqScale()
{
    return m_pfreqScaleDSP;
}

DspVarGroupClientInterface *Power1DspVarGenerator::getNominalPower()
{
    return m_pNomPower;
}

DspVarGroupClientInterface *Power1DspVarGenerator::getParameters()
{
    return m_pParameterDSP;
}
