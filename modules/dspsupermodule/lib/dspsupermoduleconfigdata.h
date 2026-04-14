#ifndef DSPSUPERMODULECONFIGDATA_H
#define DSPSUPERMODULECONFIGDATA_H

#include <QString>

namespace DSPSUPERMODULE
{

class DspSuperModuleConfigData
{
public:
    // DspSuperModule tries to piggy back on other modules interrupts (see
    // DspSuperModuleMeasProgram::setDspCmdList)
    // In case NO module causes an interrupt in a period the period tupel is
    // kept inside DSP array. In case the DSP array is full, DspSuperModule
    // causes an 'extra' interrupt.
    quint32 m_dspArrayEntrySize;
    // Module extents DSP array size
    quint32 m_periodsTotal;
};

}
#endif // DSPSUPERMODULECONFIGDATA_H
