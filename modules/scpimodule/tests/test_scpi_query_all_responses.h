#ifndef TEST_SCPI_QUERY_ALL_RESPONSES_H
#define TEST_SCPI_QUERY_ALL_RESPONSES_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_scpi_query_all_responses : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void checkScpiQueryResponse_data();
    void checkScpiQueryResponse();

private:
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_QUERY_ALL_RESPONSES_H
