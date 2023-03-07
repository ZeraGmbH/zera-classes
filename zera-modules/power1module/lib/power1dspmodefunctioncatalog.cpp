#include "power1dspmodefunctioncatalog.h"
#include "measmodecatalog.h"

ModeNameFunctionHash Power1DspModeFunctionCatalog::m_hash;

const ModeNameFunctionHash &Power1DspModeFunctionCatalog::get()
{
    if(m_hash.isEmpty()) {
        m_hash["4LW"] = { m4lw, Power1DspCmdGenerator::getCmdsMMode4LW};
        m_hash["XLW"] = { mXlw, Power1DspCmdGenerator::getCmdsMModeXLW};
    }
    return m_hash;
}

