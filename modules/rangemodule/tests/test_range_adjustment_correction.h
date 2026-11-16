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

    void testRangeAdjTiming();

    void gainCorrectionOrder0();
    void gainCorrectionOrder1();
    void gainCorrectionOrder2();
    void phaseCorrectionOrder0();
    void phaseCorrectionOrder1();
    void phaseCorrectionOrder2();
    void offsetCorrectionOrder0();
    void offsetCorrectionOrder1();

private:
    void fireActualValues(const TestDspInterfacePtr &dspInterfaceProg, double uRms, double iRms, double frequency);
    static float polynom(double x, double coeff0, double coeff1 = 0.0, double coeff2 = 0.0, double coeff3 = 0.0);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    TestDspInterfacePtr m_sampleDspInterfaceProg;
    TestDspInterfacePtr m_sampleDspInterfaceObserv;
    TestDspInterfacePtr m_sampleDspInterfaceAdj;
};

#endif // TEST_RANGE_ADJUSTMENT_CORRECTION_H
