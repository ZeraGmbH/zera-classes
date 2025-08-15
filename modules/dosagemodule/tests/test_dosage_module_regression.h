#ifndef TEST_DOSAGE_MODULE_REGRESSION_H
#define TEST_DOSAGE_MODULE_REGRESSION_H

#include <QObject>

class test_dosage_module_regression : public QObject
{
    Q_OBJECT

private slots:
    void minimalSession();
    void veinDumpInitial();


};

#endif // TEST_DOSAGE_MODULE_REGRESSION_H
