#include "test_scpi_doc_gen_smoke.h"
#include "devicesexportgenerator.h"
#include <QTest>

QTEST_MAIN(test_scpi_doc_gen_smoke)

void test_scpi_doc_gen_smoke::generateAllDocs()
{
    DevicesExportGenerator devicesExportGenerator("test_scpi_doc_gen_smoke", QStringLiteral(HTML_DOCS_PATH_TEST));
    devicesExportGenerator.exportAll();
}
