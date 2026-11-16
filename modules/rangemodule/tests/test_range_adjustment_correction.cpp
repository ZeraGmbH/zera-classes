#include "test_range_adjustment_correction.h"
#include "demodspvaluesrange.h"
#include <scpisingletransactionblocked.h>
#include <zscpi_response_definitions.h>
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QTest>

QTEST_MAIN(test_range_adjustment_correction)

// The rangemodule runs an adjustment cycle whenever the adjust interval
// (0.5s in the test session) has elapsed (!!!) and new actual values arrive:
// for every channel it queries the PCB for the gain, phase and offset
// correction of the channel's active range and writes the results to the
// DSP as 32 float groups 'GainCorrection' / 'PhaseCorrection' /
// 'OffsetCorrection' on the INJECT_RANGE_ADJUST interface.
//
// The corrections are polynomials of order 3 stored per channel/range on
// the PCB: corr(x) = c0 + c1*x + c2*x^2 + c3*x^3. Defaults after a fresh
// start are gain c0=1 (all others 0) and phase/offset all 0, which is why
// tests without injected coefficients only see vectors of 1s and 0s.
//
// The query argument ('x') differs per correction type:
//   gain:   rms * preScaling / gainKeeperForFakingRmsValues (keeper = 1 if
//           cheating/clamping is off, see test_range_to_zero_clamp_cheating)
//   offset: rms * preScaling
//   phase:  signal frequency
// Before the values are written to the DSP they are prepared per type:
//   gain:   sign flipped for inverted channel sequence, multiplied with the
//           clamping factor (1 or 1e-10)
//   offset: multiplied with preScaling (offset is a summand in the DSP)
//   phase:  unchanged


void test_range_adjustment_correction::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();

    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-range-test.json", /*initialAdjPermission=*/true);
    m_sampleDspInterfaceProg = m_testRunner->findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    m_sampleDspInterfaceAdj = m_testRunner->findDspInterfaceByType(INJECT_RANGE_ADJUST);
}

void test_range_adjustment_correction::init()
{
    initCoefficients("m0", "250V");
    initCoefficients("m4", "10A");
}

// DSP channel numbers as reported by the PCB ('dspchannel' in  mt310s2d.xml)
static constexpr int DspChannelUl1 = 0; // m0
static constexpr int DspChannelIl1 = 1; // m3
static constexpr int DspChannelUl2 = 2; // m1
static constexpr int DspChannelIl2 = 3; // m4

void test_range_adjustment_correction::gainCorrectionOrder0()
{
    constexpr double coeff0 = 0.98;
    constexpr int order = 0;
    setCorrectionCoefficients("m0", "250V", "GAIN", order, coeff0);
    setCorrectionCoefficients("m4", "10A", "GAIN", order, coeff0);

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    // Gain is queried with amplitude rms * preScaling / keeper (= rms here):
    QCOMPARE(polynom(100.0, coeff0), findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl1));
    QCOMPARE(polynom(5.0, coeff0), findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelIl2));
    // Not adjusted channel keeps the default correction (c0 = 1):
    QCOMPARE(polynom(100.0, 1.0), findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl2));
}

void test_range_adjustment_correction::gainCorrectionOrder1()
{
    constexpr double coeff1 = 0.002;
    constexpr int order = 1;
    setCorrectionCoefficients("m0", "250V", "GAIN", order, coeff1); // UL1 -> 1 + 0.002*x
    setCorrectionCoefficients("m4", "10A", "GAIN", order, coeff1); // IL2

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    // Setting only c1 keeps the default c0 = 1 of the gain correction:
    QCOMPARE(polynom(100.0, 1.0, coeff1), findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl1));
    QCOMPARE(polynom(5.0, 1.0, coeff1), findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelIl2));
}

void test_range_adjustment_correction::gainCorrectionOrder2()
{
    constexpr double coeff2 = 0.0004;
    // Setting only c2 keeps the defaults c0 = 1 and c1 = 0:
    setCorrectionCoefficients("m0", "250V", "GAIN", 2, coeff2); // UL1 -> 1 + 0.0004*x^2
    setCorrectionCoefficients("m4", "10A", "GAIN", 2, coeff2); // IL2

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    QCOMPARE(polynom(100.0, 1.0, 0.0, coeff2), findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl1));
    QCOMPARE(polynom(5.0, 1.0, 0.0, coeff2), findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelIl2));
    QCOMPARE(polynom(100.0, 1.0), findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl2));
}

void test_range_adjustment_correction::phaseCorrectionOrder0()
{
    constexpr double coeff0 = 1.5;
    setCorrectionCoefficients("m0", "250V", "PHASE", 0, coeff0); // UL1 -> constant 1.5
    setCorrectionCoefficients("m4", "10A", "PHASE", 0, coeff0); // IL2

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    // Phase is queried with the signal frequency - both channels get the
    // same correction although their rms values differ:
    QCOMPARE(polynom(50.0, coeff0), findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelUl1));
    QCOMPARE(polynom(50.0, coeff0), findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelIl2));
}

void test_range_adjustment_correction::phaseCorrectionOrder1()
{
    constexpr double coeff1 = 0.01;
    // Setting only c1 keeps the default c0 = 0 of the phase correction:
    setCorrectionCoefficients("m0", "250V", "PHASE", 1, coeff1); // UL1 -> 0.01*f
    setCorrectionCoefficients("m4", "10A", "PHASE", 1, coeff1); // IL2

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    QCOMPARE(polynom(50.0, 0.0, coeff1), findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelUl1));
    QCOMPARE(polynom(50.0, 0.0, coeff1), findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelIl2));
    QCOMPARE(polynom(50.0, 0.0), findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelUl2));
}

void test_range_adjustment_correction::phaseCorrectionOrder2()
{
    constexpr double coeff2 = 0.0001;
    // Setting only c2 keeps the defaults c0 = 0 and c1 = 0:
    setCorrectionCoefficients("m0", "250V", "PHASE", 2, coeff2); // UL1 -> 0.0001*f^2
    setCorrectionCoefficients("m4", "10A", "PHASE", 2, coeff2); // IL2

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    QCOMPARE(polynom(50.0, 0.0, 0.0, coeff2), findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelUl1));
    QCOMPARE(polynom(50.0, 0.0, 0.0, coeff2), findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelIl2));
}

void test_range_adjustment_correction::offsetCorrectionOrder0()
{
    constexpr double coeff0 = 0.05;
    setCorrectionCoefficients("m0", "250V", "OFFSET", 0, coeff0); // UL1 -> constant 0.05
    setCorrectionCoefficients("m4", "10A", "OFFSET", 0, coeff0); // IL2

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    // Offset is queried with amplitude rms * preScaling and multiplied with
    // the pre-scaling factor before being written (1 here):
    QCOMPARE(polynom(100.0, coeff0), findDspWriteValue(spyDspVarWrite, "OFFSETCORRECTION", DspChannelUl1));
    QCOMPARE(polynom(5.0, coeff0), findDspWriteValue(spyDspVarWrite, "OFFSETCORRECTION", DspChannelIl2));
}

void test_range_adjustment_correction::offsetCorrectionOrder1()
{
    // A non-constant offset correction does not occur in real adjustment
    // procedures - this just documents what the module would do with it:
    constexpr double coeff1 = 0.001;
    // Setting only c1 keeps the default c0 = 0 of the offset correction:
    setCorrectionCoefficients("m0", "250V", "OFFSET", 1, coeff1); // UL1 -> 0.001*x
    setCorrectionCoefficients("m4", "10A", "OFFSET", 1, coeff1); // IL2

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    QCOMPARE(polynom(100.0, 0.0, coeff1), findDspWriteValue(spyDspVarWrite, "OFFSETCORRECTION", DspChannelUl1));
    QCOMPARE(polynom(5.0, 0.0, coeff1), findDspWriteValue(spyDspVarWrite, "OFFSETCORRECTION", DspChannelIl2));
}


void test_range_adjustment_correction::fireActualValues(const TestDspInterfacePtr &dspInterfaceProg,
                                                        double uRms, double iRms, double frequency)
{
    DemoDspValuesRange rangeValues(8);
    rangeValues.setFrequency(frequency);
    rangeValues.setRmsPeakDCValue(0, uRms); // m0 / UL1
    rangeValues.setRmsPeakDCValue(1, uRms); // m1 / UL2
    rangeValues.setRmsPeakDCValue(2, uRms); // m2 / UL3
    rangeValues.setRmsPeakDCValue(3, iRms); // m3 / IL1
    rangeValues.setRmsPeakDCValue(4, iRms); // m4 / IL2
    rangeValues.setRmsPeakDCValue(5, iRms); // m5 / IL3
    rangeValues.setRmsPeakDCValue(6, uRms); // m6 / UAUX
    rangeValues.setRmsPeakDCValue(7, 0);    // m7 / IAUX
    dspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
}

// Pseudo-polynomial: reproduces what the mock PCB answers for
// 'CORR:<TYPE>? <x>' - the same Horner evaluation as
// AdjDataItem::getCorrection and the 6 significant digits of the SCPI (on MT310s2)
// answer (QString("%1").arg(val)) - so the returned float is bit-identical
// to the value the rangemodule receives and writes to the DSP. QCOMPARE can
// therefore compare exactly.
float test_range_adjustment_correction::polynom(double x, double coeff0, double coeff1, double coeff2, double coeff3)
{
    double result = coeff0; // getCorrection starts with Corr += c0 * 1.0
    double arg = x;
    result += coeff1 * arg;
    arg *= x;
    result += coeff2 * arg;
    arg *= x;
    result += coeff3 * arg;
    return float(QString::number(result, 'g', 6).toDouble()); // SCPI answer quantization
}

// The rangemodule writes three vectors per adjust cycle: GainCorrection,
// PhaseCorrection and OffsetCorrection. Each vector is indexed by the DSP
// channel number (see above); only the first 8 of the 32 floats are used.
float test_range_adjustment_correction::findDspWriteValue(const QSignalSpy &spyDspVarWrite,
                                                          const QString &dspVarName,
                                                          int dspChannelNr)
{
    for (int i = 0; i < spyDspVarWrite.count(); ++i) {
        TestDspVarWriteData writtenVarData = spyDspVarWrite.at(i).at(0).value<TestDspVarWriteData>();
        if (writtenVarData.varName == dspVarName)
            return writtenVarData.values[dspChannelNr];
    }
    qCritical("No DSP write for group: %s found", qPrintable(dspVarName));
    return 0.0f;
}

// Setting coefficients is sequence relevant (AdjDataItem::setCoefficient):
// writing the coefficient of a given order zeroes all higher order
// coefficients, while lower ones are kept. Therefore always call this with
// increasing orders.
void test_range_adjustment_correction::setCorrectionCoefficients(const QString &channelMName,
                                                                 const QString &rangeName,
                                                                 const QString &correctionType, // "GAIN", "PHASE" or "OFFSET"
                                                                 int order,
                                                                 double value)
{
    QString scpiCmd = QString("SENS:%1:%2:CORR:%3:COEFFICIENT:%4")
                              .arg(channelMName, rangeName, correctionType)
                              .arg(order);
    QString answer = ScpiSingleTransactionBlocked::cmd(scpiCmd, QString::number(value, 'g', 12));
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_range_adjustment_correction::initCoefficients(const QString &channelMName, const QString &rangeName)
{
    QString scpiCmd = QString("SENS:%1:%2:CORR:INIT").arg(channelMName, rangeName);
    QString answer = ScpiSingleTransactionBlocked::cmd(scpiCmd, "");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);
}
