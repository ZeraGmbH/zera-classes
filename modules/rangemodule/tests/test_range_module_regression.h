#ifndef TEST_RANGE_MODULE_REGRESSION_H
#define TEST_RANGE_MODULE_REGRESSION_H

#include <QObject>

class test_range_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectActualValues();
    void injectActualValuesWithPreScaling();
    void injectActualValuesWithCheatingDisabled();
    void injectActualValuesWithCheatingEnabled();
    void injectActualValuesWithCheatingAndRangeChanged();
    void injectIncreasingActualValuesWithCheatingEnabled();
    void injectActualValuesCheatingEnabledWithPreScaling();
    void dumpDspSetup();
};

#endif // TEST_RANGE_MODULE_REGRESSION_H
