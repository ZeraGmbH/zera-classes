#ifndef TEST_ADJ_MODULE_REGRESSION_H
#define TEST_ADJ_MODULE_REGRESSION_H

#include "modulemanagertestrunner.h"
#include "scpimodulenetclientblocked.h"
#include <QObject>

class test_adj_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void minimalSession();
    void veinDumpInitial();

    void adjInitWithPermission();
    void adjInitWithoutPermission();

private:
    void destroyCommonTestRunner();

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleNetClientBlocked> m_scpiClient;
};

#endif // TEST_ADJ_MODULE_REGRESSION_H
