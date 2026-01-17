#ifndef TEST_SAMPLE_SET_PLL_AUTOMATIC_H
#define TEST_SAMPLE_SET_PLL_AUTOMATIC_H

#include "demovaluesdsprange.h"
#include "modulemanagertestrunner.h"
#include <QObject>

class test_sample_set_pll_automatic : public QObject
{
    Q_OBJECT
private slots:
    void rejectOnFixedFrequency();
    void startActivatedAndFireValues();
    void activateOnNoValuesFired();
private:
    void fireRangeValues(ModuleManagerTestRunner &testRunner,
                         float rmsValue,
                         QList<int> activePhaseIdxs,
                         DemoValuesDspRange &dspRangeValues);
};

#endif // TEST_SAMPLE_SET_PLL_AUTOMATIC_H
