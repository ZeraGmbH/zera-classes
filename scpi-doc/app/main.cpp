#include "devicesexportgenerator.h"
#include "scpidocshtmlgenerator.h"
#include "modulemanagerconfig.h"
#include "mocklxdmsessionchangeparamgenerator.h"
#include <QCoreApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption zenuxVersion("z", "Specify a Zenux release version after -z", "value");
    parser.addOption(zenuxVersion);
    parser.process(a);

    QString zenuxRelease = parser.value(zenuxVersion);
    QString devIfaceXmlsPath = QStringLiteral(EXPORT_DOCS_PATH) + "scpi-xmls/";
    QString htmlOutPath = QStringLiteral(EXPORT_DOCS_PATH) + "html-docs/";
    QString sessionMapJsonPath = QStringLiteral(EXPORT_DOCS_PATH) + "SessionNamesMapping.json";

    DevicesExportGenerator devicesExportGenerator(MockLxdmSessionChangeParamGenerator::generateDemoSessionChanger());

    devicesExportGenerator.exportAll(devIfaceXmlsPath, QStringLiteral(EXPORT_DOCS_PATH) + "snapshots/"); //snapshots part is to be ignored
    ScpiDocsHtmlGenerator::createScpiDocHtmls(ModulemanagerConfig::getConfigFileNameFull(),
                                              zenuxRelease,
                                              htmlOutPath,
                                              devIfaceXmlsPath,
                                              sessionMapJsonPath);
}
