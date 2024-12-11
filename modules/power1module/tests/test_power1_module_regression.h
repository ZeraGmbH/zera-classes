#ifndef TEST_POWER1_MODULE_REGRESSION_H
#define TEST_POWER1_MODULE_REGRESSION_H

#include <QObject>

class test_power1_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void injectActualValues();
    void testScpiCommandsDisabled();
    void dumpDspSetup();
};

#endif // TEST_POWER1_MODULE_REGRESSION_H
