#include "test_scpi_doc_playground.h"
#include "jsonsessionloadertest.h"
#include "modulemanagerconfigtest.h"
#include "modulemanagertest.h"
#include "modulemanagercontroller.h"
#include "vn_introspectionsystem.h"
#include "vs_veinhash.h"
#include "licensesystemmock.h"
#include "moduleeventhandler.h"
#include <QDataStream>
#include <QTest>
#include <QProcess>
#include <QDirIterator>

QTEST_MAIN(test_scpi_doc_playground)

void test_scpi_doc_playground::createAlHtmlsExists()
{
    QFile script(QStringLiteral(SCPI_DOC_PLAYGROUND_PATH) + "/create-all-htmls");
    QVERIFY(script.exists());
}

void test_scpi_doc_playground::runCreateAlHtmls()
{
    QProcess sh;
    sh.start("/bin/sh", QStringList() << QStringLiteral(SCPI_DOC_PLAYGROUND_PATH) + "/create-all-htmls");
    sh.waitForFinished();
    qInfo() << sh.readAll();

    QDir htmlOutput(QStringLiteral(SCPI_DOC_PLAYGROUND_PATH) + "/html-output");
    QVERIFY(htmlOutput.exists());
    htmlOutput.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    //uncomment these tests after xmlstarlet is installed in CI container
/*
    QCOMPARE(htmlOutput.count(), 10);
    QDirIterator outputDir(htmlOutput, QDirIterator::Subdirectories);
    while (outputDir.hasNext()) {
        QFile html(outputDir.next());
        QVERIFY(html.size() > 0);
    }
*/
}

void test_scpi_doc_playground::createDevIfaceXml()
{
    JsonSessionLoaderTest::enableTests();
    ModulemanagerConfigTest::enableTest();
    ModuleManagerTest::enableTest();
    ModulemanagerConfig::setDemoDevice("mt310s2");
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

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();

    ModuleManagerTest modMan(availableSessionList);
    modMan.setDemo(true);
    QVERIFY(modMan.loadAllAvailableModulePlugins());
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

    QDir().mkdir(QStringLiteral(SCPI_DOC_PLAYGROUND_PATH) + "/scpi-xmls2");

    for(const QString &session: availableSessionList) {
        modMan.changeSessionFile(session);
        ModuleManagerTest::feedEventLoop();
        QString currentSession = storSystem.getStoredValue(0, "Session").toString();
        QCOMPARE(currentSession, session);

        QString actDevIface = storSystem.getStoredValue(9999, "ACT_DEV_IFACE").toString();
        if(actDevIface.isEmpty()) // we have to make module resilient to this situation
            qFatal("ACT_DEV_IFACE empty - local modulemanager running???");

        QString xmlFileName = QStringLiteral(SCPI_DOC_PLAYGROUND_PATH) + "/scpi-xmls2/" + session;
        xmlFileName.replace("json", "xml");
        QFile xmlFile(xmlFileName);
        QVERIFY(xmlFile.open(QIODevice::ReadWrite));
        QTextStream data(&xmlFile);
        data << actDevIface;
        QVERIFY(xmlFile.size() > 0);
    }
}
