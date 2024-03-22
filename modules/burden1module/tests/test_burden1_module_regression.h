#ifndef TEST_BURDEN1_MODULE_REGRESSION_H
#define TEST_BURDEN1_MODULE_REGRESSION_H

#include <QObject>

class test_burden1_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void veinDumpInitial();

    void resistanceCalculation();

    void voltageBurden230V1ADefaultSettings();
    void voltageBurden100V1A60DefaultSettings();

private:
    float calcUBurdenSb(double voltage, double current, double nominalVoltage, double wireLen, double crossSection);
    float calcUBurdenSn(double nominalBurden, double voltage, double current, double nominalVoltage, double wireLen, double crossSection);
    float calcCos(double angle);
};

#endif // TEST_BURDEN1_MODULE_REGRESSION_H
