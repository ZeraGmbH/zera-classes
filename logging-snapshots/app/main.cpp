#include "devicesexportgenerator.h"
#include "modulemanagerconfig.h"
#include "mocklxdmsessionchangeparamgenerator.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption snapshotsPath("p", "Specify path for snapshots", "value");
    QString defaultPath = QDir::currentPath();
    snapshotsPath.setDefaultValue(defaultPath);
    parser.addOption(snapshotsPath);
    parser.process(a);

    QString path = parser.value(snapshotsPath);
    QString scpiDir = QDir::cleanPath(path + "/spciDocToBeIgnored/");
    QString snapshotsDir = QDir::cleanPath(path + "/snapshots/") ;

    DevicesExportGenerator devicesExportGenerator(MockLxdmSessionChangeParamGenerator::generateDemoSessionChanger());
    devicesExportGenerator.exportAll(scpiDir, snapshotsDir); //scpiDir part is to be ignored
}
