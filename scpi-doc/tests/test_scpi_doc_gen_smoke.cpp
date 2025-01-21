#include "test_scpi_doc_gen_smoke.h"
#include "devicesexportgenerator.h"
#include "scpidocshtmlgenerator.h"
#include <QTest>

QTEST_MAIN(test_scpi_doc_gen_smoke)

void test_scpi_doc_gen_smoke::generateAllDocs()
{
    QString devIfaceXmlsPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "scpi-xmls/";
    DevicesExportGenerator devicesExportGenerator(devIfaceXmlsPath);
    devicesExportGenerator.exportAll();
    ScpiDocsHtmlGenerator::createScpiDocHtmls("test_scpi_doc_gen_smoke", QStringLiteral(HTML_DOCS_PATH_TEST), devIfaceXmlsPath);
}
