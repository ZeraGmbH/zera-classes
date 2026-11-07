#include "test_scpi_local_file_read.h"
#include "scpilocalfileread.h"
#include "scpilocalfileerrormessages.h"
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_scpi_local_file_read)

using namespace SCPIMODULE;

static void writeTestFile(const QString &filePath, const QByteArray &content)
{
    QFile file(filePath);
    Q_ASSERT(file.open(QIODevice::WriteOnly));
    Q_ASSERT(file.write(content) == content.size());
}

void test_scpi_local_file_read::readSimpleFile()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath));
    writeTestFile(basePath + "/hello.txt", "hello world\n");

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "hello.txt"), QString("hello world\n"));
}

void test_scpi_local_file_read::readExampleFileWithUmlautsAndChineseText()
{
    const QString exampleUtf8 = QString::fromUtf8(TestLogHelpers::loadFile(SCPI_LOCAL_FILE_PATH "/scpi-file-example.txt"));
    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(SCPI_LOCAL_FILE_PATH, "scpi-file-example.txt"), exampleUtf8);
}

void test_scpi_local_file_read::readFileInSubdirectory()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath + "/subdir"));
    writeTestFile(basePath + "/subdir/nested.txt", "nested content\n");

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "subdir/nested.txt"), QString("nested content\n"));
}

void test_scpi_local_file_read::rejectPathTraversalWithDotDot()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath));
    writeTestFile(tmpDir.path() + "/outside.txt", "top secret\n");

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "../outside.txt"), ScpiLocalFileErrorMessages::InvalidFilePath);
    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, ".."), ScpiLocalFileErrorMessages::InvalidFilePath);
}

void test_scpi_local_file_read::rejectAbsoluteFilePaths()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath));

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "/etc/passwd"), ScpiLocalFileErrorMessages::InvalidFilePath);
}

void test_scpi_local_file_read::rejectSymlinkEscapingBaseDirectory()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath));
    writeTestFile(tmpDir.path() + "/outside.txt", "top secret\n");
    QVERIFY(QFile("../outside.txt").link(basePath + "/link-out"));

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "link-out"), ScpiLocalFileErrorMessages::InvalidFilePath);
}

void test_scpi_local_file_read::allowSymlinkInsideBaseDirectory()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath + "/subdir"));
    writeTestFile(basePath + "/subdir/nested.txt", "nested content\n");
    QVERIFY(QFile("subdir/nested.txt").link(basePath + "/link-in"));

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "link-in"), QString("nested content\n"));
}

void test_scpi_local_file_read::rejectBrokenSymlink()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath));
    QVERIFY(QFile("missing-target").link(basePath + "/broken-link"));

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "broken-link"), ScpiLocalFileErrorMessages::FileNotFound);
}

void test_scpi_local_file_read::rejectWrongFileName()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath));

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "does-not-exist.txt"), ScpiLocalFileErrorMessages::FileNotFound);
}

void test_scpi_local_file_read::rejectSubdirectorySelected()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath + "/subdir"));

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "subdir"), ScpiLocalFileErrorMessages::NotARegularFile);
}

void test_scpi_local_file_read::rejectEmptyFileName()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath));

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, ""), ScpiLocalFileErrorMessages::InvalidFilePath);
}

void test_scpi_local_file_read::rejectFileTooLarge()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath));
    {
        QFile bigFile(basePath + "/big.bin");
        QVERIFY(bigFile.open(QIODevice::WriteOnly));
        const QByteArray filler(1024, 'A');
        for (int i = 0; i < 1025; ++i) // 1 MiB + 1 KiB
            bigFile.write(filler);
    }

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "big.bin"), ScpiLocalFileErrorMessages::FileTooLarge);
}

void test_scpi_local_file_read::rejectInvalidUtf8File()
{
    QTemporaryDir tmpDir;
    const QString basePath = tmpDir.path() + "/base";
    QVERIFY(QDir().mkpath(basePath));
    writeTestFile(basePath + "/invalid-utf8.bin", QByteArray::fromHex("FFFE"));

    QCOMPARE(ScpiLocalFileRead::readLocalFileContent(basePath, "invalid-utf8.bin"), ScpiLocalFileErrorMessages::InvalidUtf8);
}
