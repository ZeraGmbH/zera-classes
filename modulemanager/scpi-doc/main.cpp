#include "jsonsessionloadertest.h"
#include "modulemanagerconfigtest.h"
#include "modulemanagertest.h"
#include "modulemanagercontroller.h"
#include "vn_introspectionsystem.h"
#include "vs_veinhash.h"
#include "licensesystemmock.h"
#include "moduleeventhandler.h"
#include <QDataStream>
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

    JsonSessionLoaderTest::enableTests();
    ModulemanagerConfigTest::enableTest();
    ModuleManagerTest::pointToSourceSessionFiles();

    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<float> >("QList<float>");
    qRegisterMetaTypeStreamOperators<QList<double> >("QList<double>");
    qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
    qRegisterMetaTypeStreamOperators<QVector<QString> >("QVector<QString>");
    qRegisterMetaTypeStreamOperators<QList<QVariantMap> >("QList<QVariantMap>");
    qputenv("QT_FATAL_CRITICALS", "1"); \

    ModuleEventHandler evHandler;
    ModuleManagerController mmController;
    VeinNet::IntrospectionSystem introspectionSystem;
    VeinStorage::VeinHash storSystem;
    LicenseSystemMock licenseSystem;

    QList<VeinEvent::EventSystem*> subSystems;

    subSystems.append(&mmController);
    subSystems.append(&introspectionSystem);
    subSystems.append(&storSystem);
    subSystems.append(&licenseSystem);
    evHandler.setSubsystems(subSystems);

    QStringList mtSessions, comSessions;
    QStringList allSessions = QString(SESSION_FILES).split(",");
    for(QString &session: allSessions) {
        session = session.section('/', -1);
        if(session.contains("mt310s2"))
            mtSessions.append(session);
        if(session.contains("com5003"))
            comSessions.append(session);
    }

    ModuleManagerTest modMan(allSessions);
    modMan.setDemo(true);
    modMan.loadAllAvailableModulePlugins();
    modMan.setStorage(&storSystem);
    modMan.setLicenseSystem(&licenseSystem);
    modMan.setEventHandler(&evHandler);
    mmController.setStorage(&storSystem);

    JsonSessionLoader sessionLoader;

    QObject::connect(&sessionLoader, &JsonSessionLoader::sigLoadModule, &modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigSessionSwitched, &sessionLoader, &JsonSessionLoader::loadSession);

    QObject::connect(&modMan, &ZeraModules::ModuleManager::sigModulesLoaded, &mmController, &ModuleManagerController::initializeEntity);
    QObject::connect(&mmController, &ModuleManagerController::sigChangeSession, &modMan, &ZeraModules::ModuleManager::changeSessionFile);
    QObject::connect(&mmController, &ModuleManagerController::sigModulesPausedChanged, &modMan, &ZeraModules::ModuleManager::setModulesPaused);

    mmController.initOnce();

    QDir().mkdir(QStringLiteral(SCPI_DOC_BUILD_PATH) + "/scpi-xmls");

    generateDevIfaceForAllSessions(mtSessions, &modMan, &storSystem);
    modMan.destroyModules();
    do
        ModuleManagerTest::feedEventLoop();
    while(!modMan.areAllModulesShutdown());
    modMan.changeMockServices("com5003");
    generateDevIfaceForAllSessions(comSessions, &modMan, &storSystem);

    QProcess sh;
    sh.start("/bin/sh", QStringList() << QStringLiteral(SCPI_DOC_SOURCE_PATH) + "/xml-to-html/create-all-htmls" << QStringLiteral(SCPI_DOC_BUILD_PATH));
    sh.waitForFinished();
    printf("%s", qPrintable(sh.readAll()));
}
