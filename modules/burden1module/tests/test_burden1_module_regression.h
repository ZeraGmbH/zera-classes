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
};

#endif // TEST_BURDEN1_MODULE_REGRESSION_H
