#include "power1dspvargenerator.h"
#include "measmodeinfo.h"

Power1DspVarGenerator::Power1DspVarGenerator()
{

}

void Power1DspVarGenerator::setupVarList(Zera::cDSPInterface *pDSPInterFace, const POWER1MODULE::cPower1ModuleConfigData *confData, quint32 sampleRate)
{
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = pDSPInterFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addDspVar("MEASSIGNAL1", sampleRate, DSPDATA::vDspTemp); // we need 2 signals for our computations
    m_pTmpDataDsp->addDspVar("MEASSIGNAL2", sampleRate, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("VALPQS", MeasPhaseCount+SumValueCount, DSPDATA::vDspTemp); // here x1, x2, x3 , xs will land
    m_pTmpDataDsp->addDspVar("TEMP1", 2, DSPDATA::vDspTemp); // we need 2 temp. vars also for complex
    m_pTmpDataDsp->addDspVar("TEMP2", 2, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("CONST_HALF", 1, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("CONST_1_DIV_SQRT3", 1, DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("FILTER", 2*(MeasPhaseCount+SumValueCount), DSPDATA::vDspTemp);
    m_pTmpDataDsp->addDspVar("N",1,DSPDATA::vDspTemp);
    quint32 tmpDataSize = m_pTmpDataDsp->getSize();

    // a handle for parameter
    m_pParameterDSP =  pDSPInterFace->getMemHandle("Parameter");
    m_pParameterDSP->addDspVar("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt); // integrationtime res = 1ms or period
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addDspVar("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt);
    m_pParameterDSP->addDspVar("MMODE",1, DSPDATA::vDspParam, DSPDATA::dInt);
    // for meas modes with phase wise enable disable
    for(int phase=0; phase<MeasPhaseCount; phase++) {
        QString varName = QString("XMMODEPHASE%1").arg(phase);
        m_pParameterDSP->addDspVar(varName, 1, DSPDATA::vDspParam, DSPDATA::dInt);
    }
    quint32 paramDataSize = m_pParameterDSP->getSize();

    // a handle for filtered actual values
    m_pActualValuesDSP = pDSPInterFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addDspVar("VALPQSF", MeasPhaseCount+SumValueCount, DSPDATA::vDspResult);
    quint32 actualDataSize = m_pActualValuesDSP->getSize();

    // and one for the frequency output scale values, we need 1 value for each configured output
    m_pfreqScaleDSP = pDSPInterFace->getMemHandle("FrequencyScale");
    m_pfreqScaleDSP->addDspVar("FREQSCALE", confData->m_nFreqOutputCount, DSPDATA::vDspParam);
    quint32 freqScaleSize = m_pfreqScaleDSP->getSize();

    // and one for nominal power in case that measuring mode is qref
    m_pNomPower = pDSPInterFace->getMemHandle("QRefScale");
    m_pNomPower->addDspVar("NOMPOWER", 1, DSPDATA::vDspParam);
    quint32 nomPowerSize = m_pfreqScaleDSP->getSize();

    m_nDspMemUsed =
            tmpDataSize +
            paramDataSize +
            actualDataSize +
            freqScaleSize +
            nomPowerSize;
}

cDspMeasData *Power1DspVarGenerator::getActualValues()
{
    return m_pActualValuesDSP;
}

cDspMeasData *Power1DspVarGenerator::getFreqScale()
{
    return m_pfreqScaleDSP;
}

cDspMeasData *Power1DspVarGenerator::getNominalPower()
{
    return m_pNomPower;
}

cDspMeasData *Power1DspVarGenerator::getParameters()
{
    return m_pParameterDSP;
}

quint32 Power1DspVarGenerator::getMemUsed()
{
    return m_nDspMemUsed;
}
