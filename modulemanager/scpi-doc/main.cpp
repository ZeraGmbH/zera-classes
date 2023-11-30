#include "modulemanagertest.h"
#include "licensesystemmock.h"
#include <QCoreApplication>
#include <QDir>
#include <QProcess>

void generateDevIfaceForAllSessions(QStringList sessionsList, ModuleManagerTest *modMan, VeinStorage::VeinHash *storSystem)
{
    for(const QString &session: qAsConst(sessionsList)) {
        modMan->changeSessionFile(session);
        do
            ModuleManagerTest::feedEventLoop();
        while(!modMan->areAllModulesShutdown());

        QString actDevIface = storSystem->getStoredValue(9999, "ACT_DEV_IFACE").toString();
        if(actDevIface.isEmpty()) // we have to make module resilient to this situation
            qFatal("ACT_DEV_IFACE empty - local modulemanager running???");

        QString xmlFileName = QStringLiteral(SCPI_DOC_BUILD_PATH) + "/scpi-xmls/" + session;
        xmlFileName.replace("json", "xml");
        QFile xmlFile(xmlFileName);
        xmlFile.open(QFile::ReadWrite);
        QTextStream data(&xmlFile);
        data << actDevIface;
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(!QString(ZC_SERVICES_IP).isEmpty()) {
        fprintf(stderr, "Running in demo mode and ZC_SERIVCES_IP is set to %s. ZC_SERIVCES_IP must be empty in demo mode!\n", ZC_SERVICES_IP);
        return -ENODEV;
    }

    ModuleManagerTest::enableTest();
    ModuleManagerTest::pointToSourceSessionFiles();
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    qputenv("QT_FATAL_CRITICALS", "1"); \

    LicenseSystemMock licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);

    QStringList mtSessions, comSessions;
    QStringList allSessions = QString(SESSION_FILES).split(",");
    for(QString &session: allSessions) {
        session = session.section('/', -1);
        if(session.contains("mt310s2"))
            mtSessions.append(session);
        if(session.contains("com5003"))
            comSessions.append(session);
    }

    ModuleManagerTest modMan(&modManSetupFacade, true);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();

    QDir().mkdir(QStringLiteral(SCPI_DOC_BUILD_PATH) + "/scpi-xmls");

    generateDevIfaceForAllSessions(mtSessions, &modMan, modManSetupFacade.getStorageSystem());
    modMan.destroyModules();
    do
        ModuleManagerTest::feedEventLoop();
    while(!modMan.areAllModulesShutdown());
    modMan.changeMockServices("com5003");
    generateDevIfaceForAllSessions(comSessions, &modMan, modManSetupFacade.getStorageSystem());

    QProcess sh;
    sh.start("/bin/sh", QStringList() << QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/xml-to-html/create-all-htmls" << QStringLiteral(SCPI_DOC_BUILD_PATH));
    sh.waitForFinished();
    printf("%s", qPrintable(sh.readAll()));
}
