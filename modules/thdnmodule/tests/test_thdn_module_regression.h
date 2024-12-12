#ifndef TEST_THDN_MODULE_REGRESSION_H
#define TEST_THDN_MODULE_REGRESSION_H

#include <QObject>

class test_thdn_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void dumpDspSetup();
};

#endif // TEST_THDN_MODULE_REGRESSION_H
