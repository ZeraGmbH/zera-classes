#include "power1dspvargenerator.h"
#include "measmodeinfo.h"

Power1DspVarGenerator::Power1DspVarGenerator()
{

}

void Power1DspVarGenerator::setupVarList(Zera::cDSPInterface *pDSPInterFace, const POWER1MODULE::cPower1ModuleConfigData *confData, quint32 sampleRate)
{
    // we fetch a handle for sampled data and other temporary values
    m_pTmpDataDsp = pDSPInterFace->getMemHandle("TmpData");
    m_pTmpDataDsp->addVarItem(new DspVariable("MEASSIGNAL1", sampleRate, DSPDATA::vDspTemp)); // we need 2 signals for our computations
    m_pTmpDataDsp->addVarItem(new DspVariable("MEASSIGNAL2", sampleRate, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new DspVariable("VALPQS", MeasPhaseCount+SumValueCount, DSPDATA::vDspTemp)); // here x1, x2, x3 , xs will land
    m_pTmpDataDsp->addVarItem(new DspVariable("TEMP1", 2, DSPDATA::vDspTemp)); // we need 2 temp. vars also for complex
    m_pTmpDataDsp->addVarItem(new DspVariable("TEMP2", 2, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new DspVariable("FAK", 1, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new DspVariable("FILTER", 2*(MeasPhaseCount+SumValueCount), DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new DspVariable("N",1,DSPDATA::vDspTemp));
    quint32 tmpDataSize = m_pTmpDataDsp->getSize();

    // a handle for parameter
    m_pParameterDSP =  pDSPInterFace->getMemHandle("Parameter");
    m_pParameterDSP->addVarItem( new DspVariable("TIPAR",1, DSPDATA::vDspParam, DSPDATA::dInt)); // integrationtime res = 1ms or period
    // we use tistart as parameter, so we can finish actual measuring interval bei setting 0
    m_pParameterDSP->addVarItem( new DspVariable("TISTART",1, DSPDATA::vDspParam, DSPDATA::dInt));
    m_pParameterDSP->addVarItem( new DspVariable("MMODE",1, DSPDATA::vDspParam, DSPDATA::dInt));
    // for meas modes with phase wise enable disable
    for(int phase=0; phase<MeasPhaseCount; phase++) {
        QString varName = QString("XMMODEPHASE%1").arg(phase);
        m_pParameterDSP->addVarItem(new DspVariable(varName, 1, DSPDATA::vDspParam, DSPDATA::dInt));
    }
    quint32 paramDataSize = m_pParameterDSP->getSize();

    // a handle for filtered actual values
    m_pActualValuesDSP = pDSPInterFace->getMemHandle("ActualValues");
    m_pActualValuesDSP->addVarItem(new DspVariable("VALPQSF", MeasPhaseCount+SumValueCount, DSPDATA::vDspResult));
    quint32 actualDataSize = m_pActualValuesDSP->getSize();

    // and one for the frequency output scale values, we need 1 value for each configured output
    m_pfreqScaleDSP = pDSPInterFace->getMemHandle("FrequencyScale");
    m_pfreqScaleDSP->addVarItem(new DspVariable("FREQSCALE", confData->m_nFreqOutputCount, DSPDATA::vDspParam));
    quint32 freqScaleSize = m_pfreqScaleDSP->getSize();

    // and one for nominal power in case that measuring mode is qref
    m_pNomPower = pDSPInterFace->getMemHandle("QRefScale");
    m_pNomPower->addVarItem(new DspVariable("NOMPOWER", 1, DSPDATA::vDspParam));
    quint32 nomPowerSize = m_pfreqScaleDSP->getSize();

    m_nDspMemUsed =
            tmpDataSize +
            paramDataSize +
            actualDataSize +
            freqScaleSize +
            nomPowerSize;
}

void Power1DspVarGenerator::deleteVarList(Zera::cDSPInterface *pDSPInterFace)
{
    pDSPInterFace->deleteMemHandle(m_pTmpDataDsp);
    pDSPInterFace->deleteMemHandle(m_pParameterDSP);
    pDSPInterFace->deleteMemHandle(m_pActualValuesDSP);
}

DspMemoryGroups *Power1DspVarGenerator::getActualValues()
{
    return m_pActualValuesDSP;
}

DspMemoryGroups *Power1DspVarGenerator::getFreqScale()
{
    return m_pfreqScaleDSP;
}

DspMemoryGroups *Power1DspVarGenerator::getNominalPower()
{
    return m_pNomPower;
}

DspMemoryGroups *Power1DspVarGenerator::getParameters()
{
    return m_pParameterDSP;
}

quint32 Power1DspVarGenerator::getMemUsed()
{
    return m_nDspMemUsed;
}
