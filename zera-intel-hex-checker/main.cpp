#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <intelhexfileio.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("0.0.1");

    QCommandLineParser parser;
    // option for validation check of intel-hex-files
    QCommandLineOption cmdCheckIntelFileOption(QStringList() << "i" << "intel-hex-file", "Check validity of Intel-hex file", "hex filename");
    parser.addOption(cmdCheckIntelFileOption);
    parser.process(app);

    QString hexFileName = parser.value(cmdCheckIntelFileOption);
    bool ok = !hexFileName.isEmpty();
    if(ok) {
        cIntelHexFileIO hexIntelHexData;
        ok = hexIntelHexData.ReadHexFile(hexFileName);
        if(!ok) {
            QStringList warningList = hexIntelHexData.getErrorsWarnings().split('\n', Qt::SkipEmptyParts);
            for(auto str : warningList) {
                qWarning(qPrintable(str), "");
            }
        }
    }
    else {
        qWarning("No filename set in -i parameter!");
        parser.showHelp();
    }
    return ok ? 0 : -1;//app.exec();
}
