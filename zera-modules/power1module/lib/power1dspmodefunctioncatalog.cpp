#include "power1dspmodefunctioncatalog.h"
#include "measmodecatalog.h"
#include "measmodephasesetstrategy4wire.h"
#include "measmodephasesetstrategy2wire.h"
#include "measmodephasesetstrategyphasesvar.h"
#include "measmodephasesetstrategyphasesfixed.h"
#include "measmodephasesetstrategyxwire.h"

ModeNameFunctionHash Power1DspModeFunctionCatalog::m_hash;

const ModeNameFunctionHash &Power1DspModeFunctionCatalog::get(int measSytemCount)
{
    if(m_hash.isEmpty()) {
        m_hash["4LW"] = { mXlw, Power1DspCmdGenerator::getCmdsMModeXLW,
                []{ return std::make_unique<MeasModePhaseSetStrategy4Wire>();} };
        m_hash["3LW"] = { m3lw, Power1DspCmdGenerator::getCmdsMMode3LW,
                []{ return std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("101"), 3);} };
        m_hash["2LW"] = { mXlw, Power1DspCmdGenerator::getCmdsMModeXLW,
                []{ return std::make_unique<MeasModePhaseSetStrategy2Wire>();} };
        m_hash["XLW"] = { mXlw, Power1DspCmdGenerator::getCmdsMModeXLW,
                [measSytemCount] {return std::make_unique<MeasModePhaseSetStrategyXWire>(MModePhaseMask("111"), measSytemCount);}};

        m_hash["XLB"] = { mXlb, Power1DspCmdGenerator::getCmdsMModeXLB,
                [measSytemCount] {return std::make_unique<MeasModePhaseSetStrategyXWire>(MModePhaseMask("111"), measSytemCount);}};

        m_hash["XLS"] = { mXls, Power1DspCmdGenerator::getCmdsMModeXLS,
                         [measSytemCount] {return std::make_unique<MeasModePhaseSetStrategyXWire>(MModePhaseMask("111"), measSytemCount);}};
    }
    return m_hash;
}

