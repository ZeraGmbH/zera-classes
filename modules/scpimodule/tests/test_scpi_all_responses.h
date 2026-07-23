#ifndef TEST_SCPI_ALL_RESPONSES_H
#define TEST_SCPI_ALL_RESPONSES_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_scpi_all_responses : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    /* These tests are about checking all corners to see if a reponse is received.
     * They are not about contents received */

    void checkScpiQueryResponse_data();
    void checkScpiQueryResponse();

    void checkScpiQueryEmptyResponse();

    void checkScpiMulipleTransactionQueryResponse_data();
    void checkScpiMulipleTransactionQueryResponse();

    void checkNestedMeasureQueries();

    void checkScpiCmdResponse_data();
    void checkScpiCmdResponse();

    void checkScpiMulipleTransactionCmdResponse_data();
    void checkScpiMulipleTransactionCmdResponse();

private:
    void restartServerForReproducabilityWithActualValues();
    QStringList getAllScpiQueriesFromDevIface();
    QStringList getAllScpiCommandsWithParamFromDevIface();
    bool ignoreToSpeedup(const QString &scpiPath);
    QString scpiShortHeader(const QString scpiCmd);
    void addTestRow(const QStringList scpiTransaction);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_ALL_RESPONSES_H
