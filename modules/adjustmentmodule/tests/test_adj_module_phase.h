#ifndef TEST_ADJ_MODULE_PHASE_H
#define TEST_ADJ_MODULE_PHASE_H

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
    void absWithinRangeLower();
    void absOutOfRangeUpper();
    void absWithinRangeUpper();

    void outOfLimitLowerIL1();
    void outOfLimitLowerIL1Neg();
    void outOfLimitUpperIL1();
    void outOfLimitUpperIL1Neg();
    void oneNodeWithinLimitLowerIL1();
    void oneNodeWithinLimitLowerIL1Neg();
    void oneNodeWithinLimitUpperIL1();
    void oneNodeOnPointIL1();

    void outOfLimitLowerIL1_180();
    void outOfLimitUpperIL1_180();
    void oneNodewithinLimitLowerIL1_180();
    void oneNodewithinLimitLowerIL1_180Neg();
    void oneNodewithinLimitUpperIL1_180();
    void oneNodeOnPointIL1_180();
    void oneNodeOnPointIL1_180Neg();

    void outOfLimitLowerUL2();
    void outOfLimitUpperUL2();
    void withinLimitLowerUL2();
    void withinLimitUpperUL2();

    void denyRangeNotSet();
private:
    double adjustAngle(double angle);
    double adjustAngleWithNeg(double angle);
};

#endif // TEST_ADJ_MODULE_PHASE_H
