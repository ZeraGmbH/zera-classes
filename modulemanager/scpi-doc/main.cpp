#include "modulemanager.h"
#include "licensesystemmock.h"
#include "modulemanagerconfig.h"
#include <timemachineobject.h>
#include <QCoreApplication>
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

    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    qputenv("QT_FATAL_CRITICALS", "1"); \

    QDir().mkdir(QStringLiteral(HTML_DOCS_PATH) + "/scpi-xmls");
    generateDevIfaceXmls("mt310s2");
    generateDevIfaceXmls("com5003");

    printf("Supporting files location: %s\n", qPrintable(SCPI_DOC_SOURCE_PATH));
    QString createAllHtmls = QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/xml-to-html/create-all-htmls";
    if (QFile::exists(createAllHtmls)) {
        printf("create-all-htmls found !\n");
        QProcess sh;
        sh.start("/bin/sh", QStringList() << QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/xml-to-html/create-all-htmls" << QStringLiteral(HTML_DOCS_PATH));
        sh.waitForFinished();
        printf("%s", qPrintable(sh.readAll()));
    }
    else
        printf("create-all-htmls not found\n");

    QDir dir(QStringLiteral(HTML_DOCS_PATH) + "/scpi-xmls");
    dir.removeRecursively();
}
