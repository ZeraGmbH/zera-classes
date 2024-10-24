#ifndef TEST_ADJ_MODULE_REGRESSION_H
#define TEST_ADJ_MODULE_REGRESSION_H

#include <QObject>

class test_adj_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void minimalSession();
    void veinDumpInitial();

    void dspInterfacesChange();
    void adjInitWithPermission();
    void adjInitWithoutPermission();
};

#endif // TEST_ADJ_MODULE_REGRESSION_H
