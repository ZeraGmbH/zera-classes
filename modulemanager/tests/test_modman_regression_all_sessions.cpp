#include "test_modman_regression_all_sessions.h"
#include "scpidocshtmlgenerator.h"
#include "modulemanagerconfig.h"
#include "scpimeasure.h"
#include "scpibasedelegate.h"
#include "baseinterface.h"
#include <testloghelpers.h>
#include <mocklxdmsessionchangeparamgenerator.h>
#include <proxyclient_p.h>
#include <QTest>

QTEST_MAIN(test_modman_regression_all_sessions)

void test_modman_regression_all_sessions::initTestCase()
{
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    TestModuleManager::enableTests();
    qputenv("QT_FATAL_CRITICALS", "1");
    m_openFileTracker = std::make_unique<TestOpenFileTracker>();

    m_devIfaceXmlsPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "scpi-xmls/";
    DevicesExportGenerator devicesExportGenerator(MockLxdmSessionChangeParamGenerator::generateTestSessionChanger(false));
    devicesExportGenerator.exportAll(m_devIfaceXmlsPath);
    m_veinDumps = devicesExportGenerator.getVeinDumps();
    m_instanceCountsOnModulesDestroyed = devicesExportGenerator.getInstanceCountsOnModulesDestroyed();
}

void test_modman_regression_all_sessions::allSessionsVeinDumps_data()
{
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

void test_modman_regression_all_sessions::uniqueEntityNameEntityIdPairsMt581s2()
{
    QVERIFY(checkUniqueEntityIdNames("mt581s2"));
}

void test_modman_regression_all_sessions::testGenerateScpiDocs()
{
    QString htmlOutPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "html-docs/";
    QString sessionMapJsonPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "SessionNamesMapping.json";

    ScpiDocsHtmlGenerator::createScpiDocHtmls(ModulemanagerConfig::getConfigFileNameFull(),
                                              "test_scpi_doc_gen_smoke",
                                              htmlOutPath,
                                              m_devIfaceXmlsPath,
                                              sessionMapJsonPath);

    int adjustmentHtmls = 2;
    int totalHtmlFiles = m_veinDumps.keys().count() + adjustmentHtmls;
    QDir htmlDir(QStringLiteral(HTML_DOCS_PATH_TEST) + "html-docs/");
    htmlDir.setFilter(QDir::Files);
    QCOMPARE(htmlDir.count(), totalHtmlFiles);
}

void test_modman_regression_all_sessions::checkObjectsProperlyDeleted()
{
    TestModuleManager::TModuleInstances cumulatedInstanceCounts = m_instanceCountsOnModulesDestroyed.last();

    QCOMPARE(cumulatedInstanceCounts.m_veinMetaDataCount, 0);
    QCOMPARE(cumulatedInstanceCounts.m_veinComponentCount, 0);
    QCOMPARE(cumulatedInstanceCounts.m_activistCount, 0);
    QCOMPARE(cumulatedInstanceCounts.m_serviceInterfaceCount, 0);
    QCOMPARE(cumulatedInstanceCounts.m_dspVarCount, 0);
    constexpr int licenseStorageSystemModuleIntrospection_EventSystemCount = 4;
    QCOMPARE(cumulatedInstanceCounts.m_veinEventSystemCount, licenseStorageSystemModuleIntrospection_EventSystemCount);

    constexpr int serverPeers = 10;
    QCOMPARE(cumulatedInstanceCounts.m_veinTcpPeerCount, serverPeers);
    constexpr int peersLeftServerDown = 0;
    QCOMPARE(VeinTcp::TcpPeer::getInstanceCount(), peersLeftServerDown);

    QCOMPARE(SCPIMODULE::cBaseInterface::getInstanceCount(), 0);
    QCOMPARE(SCPIMODULE::cSCPIMeasure::getInstanceCount(), 0);
    QCOMPARE(SCPIMODULE::ScpiBaseDelegate::getInstanceCount(), 0);

    QCOMPARE(ProtonetCommand::getInstanceCount(), 0); // for zenux-services
    QCOMPARE(Zera::ProxyClientPrivate::getInstanceCount(), 0);
}

void test_modman_regression_all_sessions::checkFilesProperlyClosed()
{
    const QMap<QString, int> openFilesTotal = m_openFileTracker->getOpenFilesTotal();
    for (auto iter=openFilesTotal.constBegin(); iter!=openFilesTotal.constEnd(); iter++)
        qInfo("%s accessed %i time(s)", qPrintable(iter.key()), iter.value());
    QVERIFY(openFilesTotal.count() > 0);
    const QHash<QString, int> openFiles = m_openFileTracker->getOpenFiles();
    QCOMPARE(openFiles.count(), 0);
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

    for(const QString &sessionFileName: m_veinDumps.keys()) {
        if(sessionFileName.contains(device)) {
            QJsonObject jsonDumped = QJsonDocument::fromJson(m_veinDumps.value(sessionFileName)).object();
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
        }
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
