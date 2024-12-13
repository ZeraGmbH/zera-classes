#ifndef TEST_OSCI_MODULE_REGRESSION_H
#define TEST_OSCI_MODULE_REGRESSION_H

#include <QObject>

class test_osci_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void dumpDspSetup();
};

#endif // TEST_OSCI_MODULE_REGRESSION_H
