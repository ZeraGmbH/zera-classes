#include "adjmoduletesthelper.h"
#include "adjustmentmodule.h"
#include "testdspinterface.h"
#include "testdspvalues.h"

void AdjModuleTestHelper::setActualTestValues(ModuleManagerTestRunner &testRunner,
                                              float testvoltage, float testcurrent, float testangle, float testfrequency)
{
    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    TestDspValues dspValues(dspInterfaces[DSP_INTERFACE_DFT]->getValueList());
    dspValues.setAllValuesSymmetric(testvoltage, testcurrent, testangle, testfrequency);
    dspValues.fireAllActualValues(
        dspInterfaces[DSP_INTERFACE_DFT],
        dspInterfaces[DSP_INTERFACE_FFT],
        dspInterfaces[DSP_INTERFACE_RANGE_PROGRAM], // Range is for frequency only
        dspInterfaces[DSP_INTERFACE_RMS]);
}
