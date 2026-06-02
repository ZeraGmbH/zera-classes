#ifndef TEST_ADJ_MODULE_GAIN_DC_COM5__3_REF_H
#define TEST_ADJ_MODULE_GAIN_DC_COM5__3_REF_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QObject>

class test_adj_module_gain_dc_com5003_ref : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void checkIntialRangeR10V();
    void init480VNotOk();
    void initR10VOk();

private:
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
};

#endif // TEST_ADJ_MODULE_GAIN_DC_COM5__3_REF_H
