#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <zera-json-params-structure.h>

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
    QString jsonStateInputFileName = parser.value(cmdCheckJSONInput);
    QString jsonStateOutputFileName = parser.value(cmdCheckJSONOutput);
    bool ok = !jsonStructureFileName.isEmpty();
    if(ok) {
        QFile jsonStructureFile(jsonStructureFileName);
        ok = jsonStructureFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
        if(ok) {
            QByteArray jsonStructureData = jsonStructureFile.readAll();
            jsonStructureFile.close();

            cZeraJsonParamsStructure jsonParams;
            cZeraJsonParamsStructure::ErrList errList = jsonParams.loadJson(jsonStructureData, jsonStructureFileName);
            if(errList.isEmpty() && !jsonStateOutputFileName.isEmpty()) {

            }
        }

    }
    else {
        qWarning("No filename set in -s parameter!");
        parser.showHelp(-1);
    }
    return ok ? 0 : -1;//app.exec();
}
