#ifndef TEST_RANGE_ADJUSTMENT_CORRECTION_H
#define TEST_RANGE_ADJUSTMENT_CORRECTION_H

#include "testdspinterface.h"
#include "modulemanagertestrunner.h"
#include <QSignalSpy>
#include <memory>

class test_range_adjustment_correction : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void testRangeAdjTiming();

    void gainCorrectionOrder0();
    void gainCorrectionOrder1();
    void gainCorrectionOrder2();
    void phaseCorrectionOrder0();
    void phaseCorrectionOrder1();
    void phaseCorrectionOrder2();
    void offsetCorrectionOrder0();
    void offsetCorrectionOrder1();

    void changeRanges();

private:
    void fireActualValues(const TestDspInterfacePtr &dspInterfaceProg, double uRms, double iRms, double frequency);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    TestDspInterfacePtr m_sampleDspInterfaceProg;
    TestDspInterfacePtr m_sampleDspInterfaceObserv;
    TestDspInterfacePtr m_sampleDspInterfaceAdj;
};

#endif // TEST_RANGE_ADJUSTMENT_CORRECTION_H
