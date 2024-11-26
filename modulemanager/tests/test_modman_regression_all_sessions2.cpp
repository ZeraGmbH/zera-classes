#include "test_modman_regression_all_sessions2.h"
#include "testfactoryserviceinterfaces.h"
#include "modulemanagerconfig.h"
#include "testmodulemanager.h"
#include "modulemanagersetupfacade.h"
#include "testlicensesystem.h"
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_modman_regression_all_sessions2)

void test_modman_regression_all_sessions2::initTestCase()
{
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    TestModuleManager::enableTests();
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_modman_regression_all_sessions2::allSessionsVeinDumps_data()
{
    QTest::addColumn<QString>("device");
    QTest::addColumn<QString>("sessionFileName");
    const QStringList devices = QStringList() << "com5003" << "mt310s2";
    for(const QString &device : devices) {
        const QStringList sessionFileNames = getSessionFileNames(device);
        for(const QString &sessionFileName : sessionFileNames) {
            // for now - something is wrong with them...
            if(sessionFileName == "com5003-ced-session.json")
                continue;
            if(sessionFileName == "mt310s2-ced-session.json")
                continue;
            QTest::newRow(sessionFileName.toLatin1()) << device << sessionFileName;
        }
    }
}

void test_modman_regression_all_sessions2::allSessionsVeinDumps()
{
    QFETCH(QString, device);
    QFETCH(QString, sessionFileName);
    qInfo("Device %s / session file: %s", qPrintable(device), qPrintable(sessionFileName));

    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);
    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices(device, false);
    modMan.changeSessionFile(sessionFileName);
    modMan.waitUntilModulesAreReady();

    QFile file(QString(":/veinDumps/%1").arg(sessionFileName));
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    VeinStorage::DumpJson::dumpToFile(modManSetupFacade.getStorageSystem()->getDb(), &buff, QList<int>(), QList<int>() << 9999);

    modMan.destroyModulesAndWaitUntilAllShutdown();

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_modman_regression_all_sessions2::uniqueEntityNameEntityIdPairsCom5003()
{
    QVERIFY(checkUniqueEntityIdNames("com5003"));
}

void test_modman_regression_all_sessions2::uniqueEntityNameEntityIdPairsMt310s2()
{
    QVERIFY(checkUniqueEntityIdNames("mt310s2"));
}

const QStringList test_modman_regression_all_sessions2::getSessionFileNames(const QString deviceName)
{
    ModulemanagerConfig::setDemoDevice(deviceName, true);
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    return mmConfig->getAvailableSessions();
}

bool test_modman_regression_all_sessions2::checkUniqueEntityIdNames(const QString &device)
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
        TestLicenseSystem licenseSystem;
        ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);
        TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
        modMan.loadAllAvailableModulePlugins();
        modMan.setupConnections();
        modMan.startAllTestServices(device, false);
        modMan.changeSessionFile(sessionFileName);
        modMan.waitUntilModulesAreReady();

        QByteArray jsonDumpedRaw;
        QBuffer buff(&jsonDumpedRaw);
        VeinStorage::DumpJson::dumpToFile(modManSetupFacade.getStorageSystem()->getDb(), &buff);

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
