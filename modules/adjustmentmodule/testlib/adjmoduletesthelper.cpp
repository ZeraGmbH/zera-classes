#include "adjmoduletesthelper.h"
#include "testdspvalues.h"

void AdjModuleTestHelper::setAllValuesSymmetricAc(ModuleManagerTestRunner &testRunner, float testvoltage, float testcurrent, float testangle, float testfrequency)
{
    TestDspValues dspValues(testRunner.findDspInterfaceByType(INJECT_DFT)->getValueList(), 1);
    dspValues.setAllValuesSymmetricAc(testvoltage, testcurrent, testangle, testfrequency);
    dspValues.fireAllActualValues(
        testRunner.findDspInterfaceByType(INJECT_DFT),
        testRunner.findDspInterfaceByType(INJECT_FFT),
        testRunner.findDspInterfaceByType(INJECT_RANGE_PROGRAM),
        testRunner.findDspInterfaceByType(INJECT_RMS),
        testRunner.findAllDspInterfaceByType(INJECT_POWER1),
        testRunner.findDspInterfaceByType(INJECT_OSCI)
        );
}

void AdjModuleTestHelper::setAllValuesSymmetricDc(ModuleManagerTestRunner &testRunner, float voltage, float current)
{
    TestDspValues dspValues(testRunner.findDspInterfaceByType(INJECT_DFT)->getValueList(), 0);
    dspValues.setAllValuesSymmetricDc(voltage, current);
    dspValues.fireAllActualValues(
        testRunner.findDspInterfaceByType(INJECT_DFT),
        testRunner.findDspInterfaceByType(INJECT_FFT),
        testRunner.findDspInterfaceByType(INJECT_RANGE_PROGRAM),
        testRunner.findDspInterfaceByType(INJECT_RMS),
        testRunner.findAllDspInterfaceByType(INJECT_POWER1),
        testRunner.findDspInterfaceByType(INJECT_OSCI)
        );
}

AdjModuleTestHelper::TAdjNodeValues AdjModuleTestHelper::parseNode(const QString &scpiResponse)
{
    const QStringList nodeList = scpiResponse.split(";", Qt::SkipEmptyParts);
    TAdjNodeValues node;
    node.m_correction = nodeList[0].toDouble();
    node.m_loadPoint = nodeList[1].toDouble();
    return node;
}
