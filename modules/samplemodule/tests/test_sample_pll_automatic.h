#ifndef TEST_SAMPLE_PLL_AUTOMATIC_H
#define TEST_SAMPLE_PLL_AUTOMATIC_H

#include "modulemanagertestrunner.h"
#include "demovaluesdsprange.h"
#include <QList>
#include <QObject>

class test_sample_pll_automatic : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void emitImmediateAfterStart();
    void switchOffNoEmit();
    void emitTwiceAfterStart();
    void emitTwiceAfterStartStopAndMoreEmit();

    void emitUL2IfUL1IsZero();
    void emitUL2IfUL1IsBelowAndUl2IsAbove();
    void emitIL1IfVoltagesAreBelow();
    void emitIL2IfLeadingAreBelow();
    void emitSequence();
    void allBelowDefaultsUL1();
    void emitNothingOnEmptyChannelList();

private:
    void fireRangeValues(float rmsValue, QList<int> activePhaseIdxs);
    void fireRangeValues(float rmsValue, QList<int> activePhaseIdxs, DemoValuesDspRange &dspRangeValues);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SAMPLE_PLL_AUTOMATIC_H
