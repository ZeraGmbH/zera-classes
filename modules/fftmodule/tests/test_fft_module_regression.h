#ifndef TEST_FFT_MODULE_REGRESSION_H
#define TEST_FFT_MODULE_REGRESSION_H

#include <QObject>

class test_fft_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectValues();
};

#endif // TEST_FFT_MODULE_REGRESSION_H
