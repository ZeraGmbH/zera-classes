#include "test_scpi_doc_gen_smoke.h"
#include "devicesexportgenerator.h"
#include "scpidocshtmlgenerator.h"
#include "modulemanagerconfig.h"
#include <QTest>

QTEST_MAIN(test_scpi_doc_gen_smoke)

void test_scpi_doc_gen_smoke::generateAllDocs()
{
    QString devIfaceXmlsPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "scpi-xmls/";
    QString htmlOutPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "html-docs/";
    QString sessionMapJsonPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "SessionNamesMapping.json";

    DevicesExportGenerator devicesExportGenerator(devIfaceXmlsPath);
    devicesExportGenerator.exportAll(true);
    ScpiDocsHtmlGenerator::createScpiDocHtmls(ModulemanagerConfig::getConfigFileNameFull(),
                                              "test_scpi_doc_gen_smoke",
                                              htmlOutPath,
                                              devIfaceXmlsPath,
                                              sessionMapJsonPath);
}
