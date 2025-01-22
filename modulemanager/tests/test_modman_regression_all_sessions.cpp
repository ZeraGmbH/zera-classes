#include "test_modman_regression_all_sessions.h"
#include "testfactoryserviceinterfaces.h"
#include "modulemanagerconfig.h"
#include "testmodulemanager.h"
#include "modulemanagersetupfacade.h"
#include "testlicensesystem.h"
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_modman_regression_all_sessions)

void test_modman_regression_all_sessions::initTestCase()
{
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    TestModuleManager::enableTests();
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_modman_regression_all_sessions::allSessionsVeinDumps_data()
{
    QString devIfaceXmlsPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "scpi-xmls/";
    DevicesExportGenerator devicesExportGenerator(devIfaceXmlsPath);
    devicesExportGenerator.exportAll(true);
    m_veinDumps = devicesExportGenerator.getVeinDumps();

    QTest::addColumn<QString>("sessionFileName");
    for (const QString &sessionFileName: m_veinDumps.keys())
        QTest::newRow(sessionFileName.toLatin1()) << sessionFileName;
}

void test_modman_regression_all_sessions::allSessionsVeinDumps()
{
    QFETCH(QString, sessionFileName);
    QByteArray jsonExpected = TestLogHelpers::loadFile(QString(":/veinDumps/%1").arg(sessionFileName));
    QByteArray jsonDumped = m_veinDumps.value(sessionFileName);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_modman_regression_all_sessions::uniqueEntityNameEntityIdPairsCom5003()
{
    QVERIFY(checkUniqueEntityIdNames("com5003"));
}

void test_modman_regression_all_sessions::uniqueEntityNameEntityIdPairsMt310s2()
{
    QVERIFY(checkUniqueEntityIdNames("mt310s2"));
}

const QStringList test_modman_regression_all_sessions::getSessionFileNames(const QString deviceName)
{
    ModulemanagerConfig::setDemoDevice(deviceName, true);
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    return mmConfig->getAvailableSessions();
}

bool test_modman_regression_all_sessions::checkUniqueEntityIdNames(const QString &device)
{
    struct EntityNameWithOccurance
    {
        QString entityName;
        QStringList sessionFileNames;
    };
    QMap<int, EntityNameWithOccurance> entityIdNamesInSession;
    QMap<int, QList<EntityNameWithOccurance>> entityIdNamesDouble;

    const QStringList sessionFileNames = getSessionFileNames(device);
    for(const QString &sessionFileName : sessionFileNames) {
        ModulemanagerConfig::setDemoDevice(device, true);
        TestLicenseSystem licenseSystem;
        ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);
        TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
        modMan.loadAllAvailableModulePlugins();
        modMan.setupConnections();
        modMan.startAllTestServices(device, false);
        modMan.changeSessionFile(sessionFileName);
        modMan.waitUntilModulesAreReady();

        QByteArray jsonDumpedRaw = VeinStorage::DumpJson::dumpToByteArray(modManSetupFacade.getStorageSystem()->getDb());
        QJsonObject jsonDumped = QJsonDocument::fromJson(jsonDumpedRaw).object();
        for(auto iter=jsonDumped.constBegin(); iter!=jsonDumped.constEnd(); ++iter) {
            int entityId = iter.key().toInt();
            const QString entityName = iter.value().toObject()["EntityName"].toString();
            if(!entityIdNamesInSession.contains(entityId))
                entityIdNamesInSession[entityId] = { entityName, QStringList()<<sessionFileName };
            else if(entityIdNamesInSession[entityId].entityName == entityName)
                entityIdNamesInSession[entityId].sessionFileNames.append(sessionFileName);
            else if(entityIdNamesInSession[entityId].entityName != entityName)
                entityIdNamesDouble[entityId].append({ entityName, QStringList()<<sessionFileName });
        }
        modMan.destroyModulesAndWaitUntilAllShutdown();
    }
    bool ok = entityIdNamesDouble.isEmpty();
    if(!ok) {
        for(auto iter = entityIdNamesDouble.constBegin(); iter != entityIdNamesDouble.constEnd(); ++iter) {
            int entityId = iter.key();
            QString strMsg = QString("Non unique names for entity id %1 found:").arg(entityId);
            QString nameGood = entityIdNamesInSession[entityId].entityName;
            for(const QString& sessionFileName : qAsConst(entityIdNamesInSession[entityId].sessionFileNames))
                strMsg.append(QString("\n%1 in %2").arg(nameGood, sessionFileName));
            for(const auto &doubleEntry : qAsConst(entityIdNamesDouble[entityId])) {
                for(const QString& sessionFileName : qAsConst(doubleEntry.sessionFileNames))
                    strMsg.append(QString("\n%1 in %2").arg(doubleEntry.entityName, sessionFileName));
            }
            qWarning("%s", qPrintable(strMsg));
        }
    }
    return ok;
}
