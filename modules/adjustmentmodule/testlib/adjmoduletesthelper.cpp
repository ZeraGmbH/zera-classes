#include "adjmoduletesthelper.h"
#include "testdspvalues.h"

void AdjModuleTestHelper::setActualTestValues(ModuleManagerTestRunner &testRunner,
                                              float testvoltage, float testcurrent, float testangle, float testfrequency)
{
    TestDspValues dspValues(testRunner.getDspInterface(INJECT_DFT)->getValueList());
    dspValues.setAllValuesSymmetric(testvoltage, testcurrent, testangle, testfrequency);
    dspValues.fireAllActualValues(
        testRunner.getDspInterface(INJECT_DFT),
        testRunner.getDspInterface(INJECT_FFT),
        testRunner.getDspInterface(INJECT_RANGE_PROGRAM), // Range is for frequency only
        testRunner.getDspInterface(INJECT_RMS));
}

void AdjModuleTestHelper::setAllValuesSymmetricAc(ModuleManagerTestRunner &testRunner, float testvoltage, float testcurrent, float testangle, float testfrequency)
{
    TestDspValues dspValues(testRunner.getDspInterface(INJECT_DFT)->getValueList());
    dspValues.setAllValuesSymmetricAc(testvoltage, testcurrent, testangle, testfrequency);
    dspValues.fireAllActualValues(
        testRunner.getDspInterface(INJECT_DFT),
        testRunner.getDspInterface(INJECT_FFT),
        testRunner.getDspInterface(INJECT_RANGE_PROGRAM),
        testRunner.getDspInterface(INJECT_RMS));
}

void AdjModuleTestHelper::setAllValuesSymmetricDc(ModuleManagerTestRunner &testRunner, float voltage, float current)
{
    TestDspValues dspValues(testRunner.getDspInterface(INJECT_DFT)->getValueList());
    dspValues.setAllValuesSymmetricDc(voltage, current);
    dspValues.fireAllActualValues(
        testRunner.getDspInterface(INJECT_DFT),
        testRunner.getDspInterface(INJECT_FFT),
        testRunner.getDspInterface(INJECT_RANGE_PROGRAM),
        testRunner.getDspInterface(INJECT_RMS));
}

AdjModuleTestHelper::TAdjNodeValues AdjModuleTestHelper::parseNode(const QString &scpiResponse)
{
    const QStringList nodeList = scpiResponse.split(";", Qt::SkipEmptyParts);
    TAdjNodeValues node;
    node.m_correction = nodeList[0].toDouble();
    node.m_loadPoint = nodeList[1].toDouble();
    return node;
}
