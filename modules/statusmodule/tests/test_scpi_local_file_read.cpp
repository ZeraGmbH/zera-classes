#include "test_scpi_local_file_read.h"
#include "safefileloader.h"
#include "rpcreadlocalfile.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <testloghelpers.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTest>

QTEST_MAIN(test_scpi_local_file_read)

using namespace SCPIMODULE;

static const char* LocalFilePath = OE_INSTALL_ROOT "/" SCPI_LOCAL_FILE_PATH;

void test_scpi_local_file_read::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();

    RpcReadLocalFile::setFileDirBasePath(LocalFilePath);
}

void test_scpi_local_file_read::cleanupTestCase()
{
    killModman();
}

void test_scpi_local_file_read::queryReturnsExampleFileContent()
{
    startModmanWithSession(":/status-scpi-session.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    const QString exampleUtf8 = QString::fromUtf8(TestLogHelpers::loadFile(QString(LocalFilePath) + "/scpi-file-example.txt"));
    QCOMPARE(sendReceive(client, "STATUS:DEV1:LOCFILE? scpi-file-example.txt;"), exampleUtf8);
}

void test_scpi_local_file_read::queryWithoutParameterReturnsErrorMessage()
{
    startModmanWithSession(":/status-scpi-session.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "STATUS:DEV1:LOCFILE?"), "ERROR: missing parameter. Command format accepted: 'STATUS:DEV1:LOCFILE? <filepath>;'");
}

void test_scpi_local_file_read::queryReturnsErrorMessageFromFileRead()
{
    startModmanWithSession(":/status-scpi-session.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "STATUS:DEV1:LOCFILE? does-not-exist.txt;"), SafeFileLoader::stringifyLoadResult(SafeFileLoader::FileNotFound));
}

void test_scpi_local_file_read::commandFormIsRejected()
{
    startModmanWithSession(":/status-scpi-session.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "STATUS:DEV1:LOCFILE scpi-file-example.txt;"), "");
    QCOMPARE(sendReceive(client, "*STB?"), "+4"); // 1<<STBeeQueueNotEmpty
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");
}

SCPIMODULE::cSCPIModule *test_scpi_local_file_read::getScpiModule()
{
    return qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
}

void test_scpi_local_file_read::startModmanWithSession(const QString &sessionFile)
{
    if (m_testRunner == nullptr || sessionFile != m_testRunner->getSessionFileName()) {
        killModman();
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(sessionFile);
    }
}

void test_scpi_local_file_read::killModman()
{
    m_testRunner.reset();
    TimeMachineObject::feedEventLoop();
}

QString test_scpi_local_file_read::sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi)
{
    // keep line feeds - the file content may be multiline
    return client.sendReceiveNotSorted(scpi, false);
}
