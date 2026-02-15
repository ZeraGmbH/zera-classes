#ifndef TEST_PERIODAVERAGE_MODULE_REGRESSION_H
#define TEST_PERIODAVERAGE_MODULE_REGRESSION_H

#include <QObject>

class test_periodaverage_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void veinDumpInitial();
    void dumpDspSetup();
};

#endif // TEST_PERIODAVERAGE_MODULE_REGRESSION_H
