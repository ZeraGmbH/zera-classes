#ifndef TEST_RANGE_MODULE_REGRESSION_H
#define TEST_RANGE_MODULE_REGRESSION_H

#include <QObject>

class test_range_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectActualValuesWithCheating();
    void injectActualValuesWithCheatingAndRangeChanged();
    void injectIncreasingActualValuesWithCheatingEnabled();
    void injectActualValuesCheatingEnabledWithPreScaling();
};

#endif // TEST_RANGE_MODULE_REGRESSION_H
