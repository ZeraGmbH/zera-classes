#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QJsonDocument>
#include <zera-json-params-structure.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    //app.setApplicationVersion("0.0.1");

    QCommandLineParser parser;
    // option for JSON structure
    QCommandLineOption cmdCheckJSONStructure(QStringList() << "s" << "json-struture", "JSON parameter structure file name", "json structure filename");
    parser.addOption(cmdCheckJSONStructure);
    // option for JSON state input
    QCommandLineOption cmdCheckJSONInput(QStringList() << "t" << "json-state", "JSON parameter state input file name", "json input filename");
    parser.addOption(cmdCheckJSONInput);
    // option for JSON state output
    QCommandLineOption cmdCheckJSONOutput(QStringList() << "o" << "json-output", "JSON parameter state output file name", "json output filename");
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

            QJsonObject jsonStructureRaw = QJsonDocument::fromJson(jsonStructureData).object();
            // load structure
            cZeraJsonParamsStructure jsonParamStructure;
            cZeraJsonParamsStructure::ErrList errListStructure = jsonParamStructure.loadStructure(jsonStructureRaw);

            if(errListStructure.isEmpty()) { // valid structure is mandatory for state
                QJsonObject jsonStateDataLoaded;
                if(!jsonStateInputFileName.isEmpty()) {
                    QFile jsonStateFile(jsonStateInputFileName);
                    ok = jsonStateFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
                    if(ok) {
                        QByteArray jsonStateData = jsonStateFile.readAll();
                        jsonStateFile.close();
                        jsonStateDataLoaded = QJsonDocument::fromJson(jsonStateData).object();
                        errListStructure = jsonParamStructure.validateJsonState(jsonStateDataLoaded);
                        if(!errListStructure.isEmpty()) {
                            ok = false;
                            qWarning("Errors occured validating parameter file %s", qPrintable(jsonStateInputFileName));
                            while(!errListStructure.isEmpty()) {
                                cZeraJsonParamsStructure::errEntry err = errListStructure.takeFirst();
                                qWarning("%s: %s", qPrintable(err.strID()), qPrintable(err.m_strInfo));
                            }
                        }
                    }
                    else {
                        qWarning("Could not load parameter state file %s", qPrintable(jsonStateInputFileName));
                    }
                }
                if(ok && !jsonStateOutputFileName.isEmpty()) {
                    if(jsonStateDataLoaded.isEmpty()) {
                        qInfo("Loading default parameter state");
                        jsonStateDataLoaded = jsonParamStructure.createDefaultJsonState();
                    }
                }
            }
            else {
                qWarning("Errors occured loading json param structure file %s", qPrintable(jsonStructureFileName));
                while(!errListStructure.isEmpty()) {
                    cZeraJsonParamsStructure::errEntry err = errListStructure.takeFirst();
                    qWarning("%s: %s", qPrintable(err.strID()), qPrintable(err.m_strInfo));
                }
            }
        }
        else {
            qWarning("zera-json-params-cli: %s could not be opened", qPrintable(jsonStructureFileName));
        }
    }
    else { // valid structure is mandatory for state
        qWarning("No filename set in -s parameter!");
        parser.showHelp(-1);
    }
    return ok ? 0 : -1;//app.exec();
}
