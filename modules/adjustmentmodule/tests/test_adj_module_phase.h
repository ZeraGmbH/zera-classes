#ifndef TEST_ADJ_MODULE_PHASE_H
#define TEST_ADJ_MODULE_PHASE_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_adj_module_phase : public QObject
{
    Q_OBJECT
private slots:
    void noActValuesWithPermission();
    void validActValuesWithPermission();
    void validActValuesWithoutPermission();

    // from here on with permission
    void absOutOfRangeLower();
    void absOutOfRangeUpper();

    void outOfLimitLowerIL1();
    void outOfLimitUpperIL1();
    void withinLimitLowerIL1();
    void withinLimitUpperIL1();

    void outOfLimitLowerUL2();
    void outOfLimitUpperUL2();
    void withinLimitLowerUL2();
    void withinLimitUpperUL2();
private:
    void setActualTestValues(ModuleManagerTestRunner &testRunner);
    double adjustAngle(double angle);
};

#endif // TEST_ADJ_MODULE_PHASE_H
