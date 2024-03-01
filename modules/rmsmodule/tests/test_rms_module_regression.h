#ifndef TEST_RMS_MODULE_REGRESSION_H
#define TEST_RMS_MODULE_REGRESSION_H

#include <QObject>
#include <memory>

class test_rms_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectActualValues();
    void injectActualTwice();
    void injectSymmetricValues();
};

#endif // TEST_RMS_MODULE_REGRESSION_H
