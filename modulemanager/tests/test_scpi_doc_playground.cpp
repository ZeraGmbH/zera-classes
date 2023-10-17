#include "test_scpi_doc_playground.h"
#include <QTest>
#include <QProcess>
#include <QDirIterator>

QTEST_MAIN(test_scpi_doc_playground)

void test_scpi_doc_playground::createAlHtmlsExists()
{
    QFile script(QStringLiteral(SCPI_DOC_PLAYGROUND_PATH) + "/create-all-htmls");
    QVERIFY(script.exists());
}

void test_scpi_doc_playground::runCreateAlHtmls()
{
    QProcess sh;
    sh.start("/bin/sh", QStringList() << QStringLiteral(SCPI_DOC_PLAYGROUND_PATH) + "/create-all-htmls");
    sh.waitForFinished();
    qInfo() << sh.readAll();

    QDir htmlOutput(QStringLiteral(SCPI_DOC_PLAYGROUND_PATH) + "/html-output");
    QVERIFY(htmlOutput.exists());
    htmlOutput.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    //uncomment these tests after xmlstarlet is installed in CI container
/*
    QCOMPARE(htmlOutput.count(), 10);
    QDirIterator outputDir(htmlOutput, QDirIterator::Subdirectories);
    while (outputDir.hasNext()) {
        QFile html(outputDir.next());
        QVERIFY(html.size() > 0);
    }
*/
}
