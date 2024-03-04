#ifndef TEST_ADJ_MODULE_REGRESSION_H
#define TEST_ADJ_MODULE_REGRESSION_H

#include <QObject>

class test_adj_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void veinDumpInitial();
};

#endif // TEST_ADJ_MODULE_REGRESSION_H
