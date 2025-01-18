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
    void outOfLimitUpperIL1();
    void oneNodeWithinLimitLowerIL1();
    void oneNodeWithinLimitUpperIL1();
    void oneNodeOnPointIL1();

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
