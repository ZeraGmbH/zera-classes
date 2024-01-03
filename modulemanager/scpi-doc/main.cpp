#include "modulemanager.h"
#include "licensesystemmock.h"
#include "modulemanagerconfig.h"
#include <timemachineobject.h>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QProcess>

void generateDevIfaceXmls(QString deviceName)
{
    ModulemanagerConfig::setDemoDevice(deviceName);
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    QStringList sessions = mmConfig->getAvailableSessions();
    LicenseSystemMock licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);
    ZeraModules::ModuleManager modMan(&modManSetupFacade, true);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.setMockServices(deviceName);
    for(const QString &session: qAsConst(sessions)) {
        modMan.changeSessionFile(session);
        do
            TimeMachineObject::feedEventLoop();
        while(!modMan.modulesReady());
        QString actDevIface = modManSetupFacade.getStorageSystem()->getStoredValue(9999, "ACT_DEV_IFACE").toString();
        if(actDevIface.isEmpty()) // we have to make module resilient to this situation
            qFatal("ACT_DEV_IFACE empty - local modulemanager running???");

        QString xmlFileName = QStringLiteral(HTML_DOCS_PATH) + "/scpi-xmls/" + session;
        xmlFileName.replace("json", "xml");
        QFile xmlFile(xmlFileName);
        xmlFile.open(QFile::ReadWrite);
        QTextStream data(&xmlFile);
        data << actDevIface;
    }
    modMan.destroyModules();
    do
        TimeMachineObject::feedEventLoop();
    while(!modMan.modulesReady());
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption zenuxVersion("z", "Specify a Zenux release version after -z", "value");
    parser.addOption(zenuxVersion);
    parser.process(a);
    QString zenuxRelease = parser.value(zenuxVersion);

    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    qputenv("QT_FATAL_CRITICALS", "1"); \

    QDir().mkdir(QStringLiteral(HTML_DOCS_PATH) + "/scpi-xmls");
    generateDevIfaceXmls("mt310s2");
    generateDevIfaceXmls("com5003");

    QProcess sh;
    sh.start("/bin/sh", QStringList() << QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/xml-to-html/create-all-htmls" << QStringLiteral(HTML_DOCS_PATH) << zenuxRelease);
    sh.waitForFinished();
    printf("%s", qPrintable(sh.readAll()));

    QDir dir(QStringLiteral(HTML_DOCS_PATH) + "/scpi-xmls");
    dir.removeRecursively();
}
