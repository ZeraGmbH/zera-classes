#ifndef TEST_SCPI_LOCAL_FILE_H
#define TEST_SCPI_LOCAL_FILE_H

#include "modulemanagertestrunner.h"
#include "scpimodule.h"
#include "scpitestclient.h"
#include <QTemporaryDir>

class test_scpi_local_file : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    // unit tests of the static functions (no running session needed)
    void extractFilePathParamVariants_data();
    void extractFilePathParamVariants();
    void isValidUtf8Cases_data();
    void isValidUtf8Cases();
    void getBaseDirPathDefaultAndEnvOverride();

    // integration tests through the full scpi stack (modulemanager session + test client)
    void queryReturnsExampleFileContent();
    void queryWithParamDirectlyBehindQuestionMark();
    void queryWithoutParameterReturnsErrorMessage();
    void queryWithTooManyParametersReturnsErrorMessage();
    void queryWithPathTraversalReturnsErrorMessage();
    void queryWithWrongFileNameReturnsErrorMessage();
    void queryWithSubdirectorySelectedReturnsErrorMessage();
    void commandFormIsRejected();
    void devIfaceContainsLocalFileQuery();

private:
    // creates the file structure below baseDirPath (example file, subdirectory, symlinks, ...)
    void createTestFileStructure(const QString &baseDirPath, const QString &outsideFilePath);

    SCPIMODULE::cSCPIModule *getScpiModule();
    void startModmanWithSession(const QString &sessionFile);
    void killModman();
    QString sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    QTemporaryDir m_integrationTempDir; // shared by all integration tests
    QString m_basePath;                 // <m_integrationTempDir.path()>/base
};

#endif // TEST_SCPI_LOCAL_FILE_H
