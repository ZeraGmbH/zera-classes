#include "test_scpi_local_file.h"
#include "scpimodellocalfile.h"
#include "scpilocalfileerrormessages.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <testloghelpers.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTest>

QTEST_MAIN(test_scpi_local_file)

using namespace SCPIMODULE;

static void writeTestFile(const QString &filePath, const QByteArray &content)
{
    QFile file(filePath);
    Q_ASSERT(file.open(QIODevice::WriteOnly));
    Q_ASSERT(file.write(content) == content.size());
}

void test_scpi_local_file::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();

    m_basePath = m_integrationTempDir.path() + "/base";
    createTestFileStructure(m_basePath, m_integrationTempDir.path() + "/outside.txt");
}

void test_scpi_local_file::cleanupTestCase()
{
    qunsetenv("ZERA_SCPI_LOCALFILE_BASEPATH");
    killModman();
}

// ---------------------------------------------------------------- unit tests

void test_scpi_local_file::extractFilePathParamVariants_data()
{
    QTest::addColumn<QString>("scpi");
    QTest::addColumn<QString>("expected");

    QTest::newRow("space after question mark") << "SYSTEM:LOCALFILE? scpi-file-example.txt;" << "scpi-file-example.txt";
    QTest::newRow("no trailing semicolon") << "SYSTEM:LOCALFILE? scpi-file-example.txt" << "scpi-file-example.txt";
    QTest::newRow("param directly behind question mark") << "SYSTEM:LOCALFILE?subdir/nested.txt;" << "subdir/nested.txt";
    QTest::newRow("surrounding whitespace") << "SYSTEM:LOCALFILE?  subdir/nested.txt ;" << "subdir/nested.txt";
    QTest::newRow("empty parameter") << "SYSTEM:LOCALFILE?" << "";
    QTest::newRow("only whitespace parameter") << "SYSTEM:LOCALFILE?   ;" << "";
    QTest::newRow("command form without question mark") << "SYSTEM:LOCALFILE scpi-file-example.txt;" << "";
}

void test_scpi_local_file::extractFilePathParamVariants()
{
    QFETCH(QString, scpi);
    QFETCH(QString, expected);

    QCOMPARE(ScpiModelLocalFile::extractFilePathParam(scpi), expected);
}

void test_scpi_local_file::isValidUtf8Cases_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<bool>("expected");

    QTest::newRow("empty") << QByteArray() << true;
    QTest::newRow("ascii") << QByteArray("plain ascii") << true;
    QTest::newRow("german umlauts") << QByteArray("German: Ä Ö Ü ä ö ü ß") << true;
    QTest::newRow("chinese") << QByteArray("Chinese: 你好，世界！") << true;
    QTest::newRow("emoji U+1F600") << QByteArray::fromHex("F09F9880") << true;
    QTest::newRow("max codepoint U+10FFFF") << QByteArray::fromHex("F48FBFBF") << true;
    QTest::newRow("valid 3-byte start E0A0") << QByteArray::fromHex("E0A080") << true;
    QTest::newRow("overlong 2-byte slash") << QByteArray::fromHex("C0AF") << false;
    QTest::newRow("overlong 3-byte A") << QByteArray::fromHex("E080AE") << false;
    QTest::newRow("overlong 4-byte U+0082") << QByteArray::fromHex("F0808082") << false;
    QTest::newRow("lead byte above U+10FFFF F5") << QByteArray::fromHex("F5808080") << false;
    QTest::newRow("stray continuation byte") << QByteArray::fromHex("80") << false;
    QTest::newRow("truncated 3-byte sequence") << QByteArray::fromHex("E4BD") << false;
    QTest::newRow("invalid lead byte C1") << QByteArray::fromHex("C180") << false;
}

void test_scpi_local_file::isValidUtf8Cases()
{
    QFETCH(QByteArray, data);
    QFETCH(bool, expected);

    QCOMPARE(ScpiModelLocalFile::isValidUtf8(data), expected);
}

void test_scpi_local_file::getBaseDirPathDefaultAndEnvOverride()
{
    qunsetenv("ZERA_SCPI_LOCALFILE_BASEPATH");
    QCOMPARE(ScpiModelLocalFile::getBaseDirPath(), SCPI_LOCAL_FILE_PATH);

    QTemporaryDir tmpDir;
    QVERIFY(tmpDir.isValid());
    qputenv("ZERA_SCPI_LOCALFILE_BASEPATH", tmpDir.path().toUtf8());
    QCOMPARE(ScpiModelLocalFile::getBaseDirPath(), tmpDir.path());
    qunsetenv("ZERA_SCPI_LOCALFILE_BASEPATH");
}


// ------------------------------------------------------------ integration tests

void test_scpi_local_file::queryReturnsExampleFileContent()
{
    startModmanWithSession(":/session-scpi-only.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    const QString exampleUtf8 = QString::fromUtf8(TestLogHelpers::loadFile(SCPI_LOCAL_FILE_PATH "/scpi-file-example.txt"));
    QCOMPARE(sendReceive(client, "SYSTEM:LOCALFILE? scpi-file-example.txt;"), exampleUtf8);
}

void test_scpi_local_file::queryWithParamDirectlyBehindQuestionMark()
{
    qputenv("ZERA_SCPI_LOCALFILE_BASEPATH", m_basePath.toUtf8());
    startModmanWithSession(":/session-scpi-only.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:LOCALFILE?subdir/nested.txt;"), QString("nested content\n"));
}

void test_scpi_local_file::queryWithoutParameterReturnsErrorMessage()
{
    qputenv("ZERA_SCPI_LOCALFILE_BASEPATH", m_basePath.toUtf8());
    startModmanWithSession(":/session-scpi-only.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:LOCALFILE?"), ScpiLocalFileErrorMessages::ScpiParameterMissing);
}

void test_scpi_local_file::queryWithTooManyParametersReturnsErrorMessage()
{
    qputenv("ZERA_SCPI_LOCALFILE_BASEPATH", m_basePath.toUtf8());
    startModmanWithSession(":/session-scpi-only.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:LOCALFILE? foo;bar;"), ScpiLocalFileErrorMessages::ScpiTooManyParameters);
}

void test_scpi_local_file::queryWithPathTraversalReturnsErrorMessage()
{
    qputenv("ZERA_SCPI_LOCALFILE_BASEPATH", m_basePath.toUtf8());
    startModmanWithSession(":/session-scpi-only.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:LOCALFILE? ../outside.txt;"), ScpiLocalFileErrorMessages::InvalidFilePath);
}

void test_scpi_local_file::queryWithWrongFileNameReturnsErrorMessage()
{
    qputenv("ZERA_SCPI_LOCALFILE_BASEPATH", m_basePath.toUtf8());
    startModmanWithSession(":/session-scpi-only.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:LOCALFILE? does-not-exist.txt;"), ScpiLocalFileErrorMessages::FileNotFound);
}

void test_scpi_local_file::queryWithSubdirectorySelectedReturnsErrorMessage()
{
    qputenv("ZERA_SCPI_LOCALFILE_BASEPATH", m_basePath.toUtf8());
    startModmanWithSession(":/session-scpi-only.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:LOCALFILE? subdir;"), ScpiLocalFileErrorMessages::NotARegularFile);
}

void test_scpi_local_file::commandFormIsRejected()
{
    qputenv("ZERA_SCPI_LOCALFILE_BASEPATH", m_basePath.toUtf8());
    startModmanWithSession(":/session-scpi-only.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:LOCALFILE scpi-file-example.txt;"), "");
    QCOMPARE(sendReceive(client, "*STB?"), "+4"); // 1<<STBeeQueueNotEmpty
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_local_file::devIfaceContainsLocalFileQuery()
{
    qputenv("ZERA_SCPI_LOCALFILE_BASEPATH", m_basePath.toUtf8());
    startModmanWithSession(":/session-scpi-only.json");
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    const QString dumped = sendReceive(client, "DEVICE:IFACE?");
    QVERIFY(dumped.contains("<LOCALFILE"));
    QVERIFY(dumped.contains("ScpiPath=\"SYSTEM:LOCALFILE\""));
    QVERIFY(dumped.contains("Type=\"Query,Command+Par\""));
}

// ------------------------------------------------------------------- helpers

void test_scpi_local_file::createTestFileStructure(const QString &baseDirPath, const QString &outsideFilePath)
{
    QVERIFY(QDir().mkpath(baseDirPath + "/subdir"));

    // single source of truth - the file shipped and installed by modules/scpimodule/CMakeLists.txt
    QVERIFY(QFile::copy(SCPI_FILE_EXAMPLE_SOURCE_PATH, baseDirPath + "/scpi-file-example.txt"));

    writeTestFile(baseDirPath + "/subdir/nested.txt", "nested content\n");
    writeTestFile(outsideFilePath, "top secret\n");
    writeTestFile(baseDirPath + "/invalid-utf8.bin", QByteArray::fromHex("FFFE"));
    {
        QFile bigFile(baseDirPath + "/big.bin");
        QVERIFY(bigFile.open(QIODevice::WriteOnly));
        const QByteArray filler(1024, 'A');
        for (int i = 0; i < 1025; ++i) // 1 MiB + 1 KiB
            bigFile.write(filler);
    }

    QVERIFY(QFile("../outside.txt").link(baseDirPath + "/link-out"));
    QVERIFY(QFile("subdir/nested.txt").link(baseDirPath + "/link-in"));
    QVERIFY(QFile("missing-target").link(baseDirPath + "/broken-link"));
}

SCPIMODULE::cSCPIModule *test_scpi_local_file::getScpiModule()
{
    return qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
}

void test_scpi_local_file::startModmanWithSession(const QString &sessionFile)
{
    if (m_testRunner == nullptr || sessionFile != m_testRunner->getSessionFileName()) {
        killModman();
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(sessionFile);
    }
}

void test_scpi_local_file::killModman()
{
    m_testRunner.reset();
    TimeMachineObject::feedEventLoop();
}

QString test_scpi_local_file::sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi)
{
    // keep line feeds - the file content may be multiline
    return client.sendReceiveNotSorted(scpi, false);
}
