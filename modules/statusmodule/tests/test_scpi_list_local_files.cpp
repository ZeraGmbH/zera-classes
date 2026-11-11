#include "test_scpi_list_local_files.h"
#include "rpclistlocalfiles.h"
#include <safefileloadenvironmentgenerator.h>
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <QDir>
#include <QTest>

QTEST_MAIN(test_scpi_list_local_files)

void test_scpi_list_local_files::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    SafeFileLoadEnvironmentGenerator::createTestFileStructure();
}

void test_scpi_list_local_files::cleanupTestCase()
{
    SafeFileLoadEnvironmentGenerator::cleanup();
    killModman();
}

void test_scpi_list_local_files::init()
{
    RpcListLocalFiles::setFileDirBasePath(SafeFileLoadEnvironmentGenerator::getInsidePath());
}

void test_scpi_list_local_files::queryReturnsFileNamesIncludingSubdirFiles()
{
    startModmanWithSession(":/status-scpi-session.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    // no safety filtering here - RPC_ReadLocalFile takes care of that when reading
    const QString expected = "big.bin\n"
                             "hardlink-out\n"
                             "hello.txt\n"
                             "link-in\n"
                             "link-out\n"
                             "subdir/nested.txt\n"
                             "too-big.bin";
    QCOMPARE(sendReceive(client, "STATUS:DEV1:LOCFILE:CATALOG?"), expected);
}

void test_scpi_list_local_files::queryReturnsAtMostMaxListedFiles()
{
    constexpr int lineLimit = 3;
    RpcListLocalFiles::setMaxListedFiles(lineLimit);

    startModmanWithSession(":/status-scpi-session.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    const QStringList dumpedLines = sendReceive(client, "STATUS:DEV1:LOCFILE:CATALOG?").split("\n");
    QCOMPARE(dumpedLines.count(), lineLimit);
}

void test_scpi_list_local_files::queryReturnsErrorMessageWhenDirDoesNotExist()
{
    RpcListLocalFiles::setFileDirBasePath(SafeFileLoadEnvironmentGenerator::getInsidePath() + QDir::separator() + "foo");

    startModmanWithSession(":/status-scpi-session.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "STATUS:DEV1:LOCFILE:CATALOG?"), "ERROR: file directory not found");
}

void test_scpi_list_local_files::queryReturnsEmptyStringForEmptyDirectory()
{
    const QString emptyDir = SafeFileLoadEnvironmentGenerator::getInsidePath() + QDir::separator() + "empty-dir";
    QVERIFY(QDir().mkpath(emptyDir));
    RpcListLocalFiles::setFileDirBasePath(emptyDir);

    startModmanWithSession(":/status-scpi-session.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "STATUS:DEV1:LOCFILE:CATALOG?"), "");
}

void test_scpi_list_local_files::commandFormIsRejected()
{
    startModmanWithSession(":/status-scpi-session.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "STATUS:DEV1:LOCFILE:CATALOG;"), "");
    QCOMPARE(sendReceive(client, "*STB?"), "+4"); // 1<<STBeeQueueNotEmpty
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");
}

SCPIMODULE::cSCPIModule *test_scpi_list_local_files::getScpiModule()
{
    return qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
}

void test_scpi_list_local_files::startModmanWithSession(const QString &sessionFile)
{
    if (m_testRunner == nullptr || sessionFile != m_testRunner->getSessionFileName()) {
        killModman();
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(sessionFile);
    }
}

void test_scpi_list_local_files::killModman()
{
    m_testRunner.reset();
    TimeMachineObject::feedEventLoop();
}

QString test_scpi_list_local_files::sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi)
{
    // keep line feeds - the file list is multiline
    return client.sendReceiveNotSorted(scpi, false);
}
