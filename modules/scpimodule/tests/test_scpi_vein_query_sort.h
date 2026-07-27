#ifndef TEST_SCPI_VEIN_QUERY_SORT_H
#define TEST_SCPI_VEIN_QUERY_SORT_H

#include "modulemanagertestrunner.h"
#include <memory>

class test_scpi_vein_query_sort : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void defaultEnabled();
    void switchOff();

private:
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_VEIN_QUERY_SORT_H
