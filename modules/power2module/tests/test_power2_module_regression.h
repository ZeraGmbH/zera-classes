#ifndef TEST_POWER2_MODULE_REGRESSION_H
#define TEST_POWER2_MODULE_REGRESSION_H

#include <QObject>

class test_power2_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void dumpDspSetup();
};

#endif // TEST_POWER2_MODULE_REGRESSION_H
