#ifndef TEST_SCPI_LIST_LOCAL_FILES_H
#define TEST_SCPI_LIST_LOCAL_FILES_H

#include "modulemanagertestrunner.h"
#include "scpitestclient.h"
#include "scpimodule.h"

class test_scpi_list_local_files : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

    void queryReturnsFileNamesIncludingSubdirFiles();
    void queryReturnsAtMostMaxListedFiles();
    void queryReturnsErrorMessageWhenDirDoesNotExist();
    void queryReturnsEmptyStringForEmptyDirectory();
    void commandFormIsRejected();

private:
    SCPIMODULE::cSCPIModule *getScpiModule();
    void startModmanWithSession(const QString &sessionFile);
    void killModman();
    QString sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_LIST_LOCAL_FILES_H
