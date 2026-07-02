#ifndef TEST_SCPI_ALL_RESPONSES_H
#define TEST_SCPI_ALL_RESPONSES_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_scpi_all_responses : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void checkScpiQueryResponse_data();
    void checkScpiQueryResponse();

    void checkScpiQueryEmptyResponse();

    void checkScpiMulipleTransactionQueryResponse_data();
    void checkScpiMulipleTransactionQueryResponse();

    void checkScpiCmdResponse_data();
    void checkScpiCmdResponse();

private:
    QStringList getAllScpiQueriesFromDevIface();
    QStringList getAllScpiCommandsWithParamFromDevIface();
    bool ignoreToSpeedup(const QString &scpiPath);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_ALL_RESPONSES_H
