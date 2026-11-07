#ifndef TEST_SCPI_LOCAL_FILE_READ_H
#define TEST_SCPI_LOCAL_FILE_READ_H

#include "modulemanagertestrunner.h"
#include "scpitestclient.h"
#include "scpimodule.h"

class test_scpi_local_file_read : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void queryReturnsExampleFileContent();
    void queryWithoutParameterReturnsErrorMessage();
    void queryReturnsErrorMessageFromFileRead();
    void commandFormIsRejected();

private:
    SCPIMODULE::cSCPIModule *getScpiModule();
    void startModmanWithSession(const QString &sessionFile);
    void killModman();
    QString sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    QString m_testFileInsidePath;
};

#endif // TEST_SCPI_LOCAL_FILE_READ_H
