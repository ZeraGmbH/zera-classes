#include "power1dspmodefunctioncatalog.h"
#include "measmodecatalog.h"
#include "measmodephasesetstrategy4wire.h"
#include "measmodephasesetstrategyphasesvar.h"

ModeNameFunctionHash Power1DspModeFunctionCatalog::m_hash;

const ModeNameFunctionHash &Power1DspModeFunctionCatalog::get(int measSytemCount)
{
    if(m_hash.isEmpty()) {
        m_hash["4LW"] = { m4lw, Power1DspCmdGenerator::getCmdsMMode4LW,
                []{ return std::make_unique<MeasModePhaseSetStrategy4Wire>();} };
        m_hash["XLW"] = { mXlw, Power1DspCmdGenerator::getCmdsMModeXLW,
                [measSytemCount] {return std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("111"), measSytemCount);}};
    }
    return m_hash;
}

