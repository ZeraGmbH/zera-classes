#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QRegularExpression>
#include <intelhexfileio.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("0.0.1");

    QCommandLineParser parser;
    // option for validation check of intel-hex-files
    QCommandLineOption cmdCheckIntelFileOption(QStringList() << "i" << "intel-hex-file", "Check validity of Intel-hex file", "hex filename");
    parser.addOption(cmdCheckIntelFileOption);

    QCommandLineOption cmdVerifyChecksum(QStringList() << "v" << "verify-checksum", "Verify checksum in hex file", "hex checksum");
    parser.addOption(cmdVerifyChecksum);

    parser.process(app);

    QString hexFileName = parser.value(cmdCheckIntelFileOption);
    bool ok = !hexFileName.isEmpty();
    if(ok) {
        cIntelHexFileIO hexIntelHexData;
        ok = hexIntelHexData.ReadHexFile(hexFileName);
        if(ok) {
            QString strHex = parser.value(cmdVerifyChecksum).toUpper();
            if(!strHex.isEmpty()) {
                // some validity check
                QRegularExpression regValidHex("^[0-9A-F]+$");
                QRegularExpressionMatch match = regValidHex.match(strHex);
                ok = match.hasMatch() && strHex.length() %2 == 0;
                if(ok) {
                    QByteArray verifyData = QByteArray::fromHex(strHex.toLatin1());
                    ok = hexIntelHexData.contains(verifyData);
                    if(!ok) {
                        qWarning("Checksum-check failed: %s does not contain %s",
                                 qPrintable(hexFileName), qPrintable(strHex));
                    }
                }
                else {
                    qWarning("Invalid hex to verify / odd length!");
                    parser.showHelp(-1);
                }
            }
        }
        else {
            QStringList warningList = hexIntelHexData.getErrorsWarnings().split('\n', QString::SkipEmptyParts);
            for(auto str : warningList) {
                qWarning(qPrintable(str), "");
            }
        }
    }
    else {
        qWarning("No filename set in -i parameter!");
        parser.showHelp(-1);
    }
    return ok ? 0 : -1;//app.exec();
}
