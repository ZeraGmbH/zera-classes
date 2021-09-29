#include <QCoreApplication>
#include <QCommandLineParser>
#include <zera-json-params.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    //app.setApplicationVersion("0.0.1");

    QCommandLineParser parser;
    // option for JSON structure
    QCommandLineOption cmdCheckJSONStructure(QStringList() << "s" << "json-struture", "JSON structure file name", "json structure filename");
    parser.addOption(cmdCheckJSONStructure);
    // option for JSON param input
    QCommandLineOption cmdCheckJSONInput(QStringList() << "i" << "json-input", "JSON input file name", "json input filename");
    parser.addOption(cmdCheckJSONInput);
    // option for JSON param output
    QCommandLineOption cmdCheckJSONOutput(QStringList() << "o" << "json-output", "JSON output file name (stout if not set)", "json output filename");
    parser.addOption(cmdCheckJSONOutput);
    parser.process(app);

    QString jsonStructureFileName = parser.value(cmdCheckJSONStructure);
    QString jsonInputFileName = parser.value(cmdCheckJSONInput);
    QString jsonOutputFileName = parser.value(cmdCheckJSONOutput);
    bool ok = !jsonStructureFileName.isEmpty();
    if(ok) {
        cZeraJsonParams jsonParams;
        cZeraJsonParams::ErrList errList = jsonParams.loadJsonFromFiles(jsonStructureFileName, jsonInputFileName);
        if(errList.isEmpty() && !jsonOutputFileName.isEmpty()) {

        }
    }
    else {
        qWarning("No filename set in -s parameter!");
        parser.showHelp(-1);
    }
    return ok ? 0 : -1;//app.exec();
}
