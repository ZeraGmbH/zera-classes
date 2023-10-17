#include "test_scpi_doc_playground.h"
#include <QTest>

QTEST_MAIN(test_scpi_doc_playground)

void test_scpi_doc_playground::createAlHtmlsExists()
{
    QFile script(QStringLiteral(SCPI_DOC_PLAYGROUND_PATH) + "/create-all-htmls");
    QVERIFY(script.exists());
}
