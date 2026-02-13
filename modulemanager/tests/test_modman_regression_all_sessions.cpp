#include "test_modman_regression_all_sessions.h"
#include "scpidocshtmlgenerator.h"
#include "modulemanagerconfig.h"
#include "scpimeasure.h"
#include "scpibasedelegate.h"
#include "baseinterface.h"
#include "sessionexportgenerator.h"
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
    m_snapshotJsonsPath = QStringLiteral(SNAPSHOT_JSONS_PATH_TEST) + "snapshots/";
    DevicesExportGenerator devicesExportGenerator(MockLxdmSessionChangeParamGenerator::generateTestSessionChanger(false));
    devicesExportGenerator.exportAll(m_devIfaceXmlsPath, m_snapshotJsonsPath);
    m_veinDumps = devicesExportGenerator.getVeinDumps();
    m_dspMemDumps = devicesExportGenerator.getDspMemDumps();
    m_secUnitDumps = devicesExportGenerator.getSecUnitDumps();
    m_instanceCountsOnModulesDestroyed = devicesExportGenerator.getInstanceCountsOnModulesDestroyed();
}

void test_modman_regression_all_sessions::allSessionsVeinDumps_data()
{
    QTest::addColumn<QString>("sessionFileName");
    const QStringList sessionFileNames = m_veinDumps.keys();
    for (const QString &sessionFileName : sessionFileNames)
        QTest::newRow(sessionFileName.toLatin1()) << sessionFileName;
}

void test_modman_regression_all_sessions::allSessionsVeinDumps()
{
    QFETCH(QString, sessionFileName);
    QByteArray jsonExpected = TestLogHelpers::loadFile(QString(":/veinDumps/%1").arg(sessionFileName));
    QByteArray jsonDumped = m_veinDumps.value(sessionFileName);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_modman_regression_all_sessions::allSessionsDspMemDumps_data()
{
    QTest::addColumn<QString>("sessionFileName");
    const QStringList sessionFileNames = m_veinDumps.keys();
    for (const QString &sessionFileName : sessionFileNames)
        QTest::newRow(sessionFileName.toLatin1()) << sessionFileName;
}

void test_modman_regression_all_sessions::allSessionsDspMemDumps()
{
    QFETCH(QString, sessionFileName);
    QByteArray jsonExpected = TestLogHelpers::loadFile(QString(":/dspMemDumps/%1").arg(sessionFileName));
    QByteArray jsonDumped = m_dspMemDumps.value(sessionFileName);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_modman_regression_all_sessions::allSessionsSecUnitDumps_data()
{
    QTest::addColumn<QString>("sessionFileName");
    const QStringList sessionFileNames = m_veinDumps.keys();
    for (const QString &sessionFileName : sessionFileNames)
        QTest::newRow(sessionFileName.toLatin1()) << sessionFileName;
}

void test_modman_regression_all_sessions::allSessionsSecUnitDumps()
{
    QFETCH(QString, sessionFileName);
    QByteArray jsonExpected = TestLogHelpers::loadFile(QString(":/secUnitDumps/%1").arg(sessionFileName));
    QByteArray jsonDumped = m_secUnitDumps.value(sessionFileName);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
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

void test_modman_regression_all_sessions::testGenerateScpiDocs_data()
{
    QString htmlOutPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "html-docs/";
    QString sessionMapJsonPath = QStringLiteral(HTML_DOCS_PATH_TEST) + "SessionNamesMapping.json";

    ScpiDocsHtmlGenerator::createScpiDocHtmls(ModulemanagerConfig::getConfigFileNameFull(),
                                              "test_scpi_doc_gen_smoke",
                                              htmlOutPath,
                                              m_devIfaceXmlsPath,
                                              sessionMapJsonPath);

    int adjustmentHtmls = 2;
    int changeInfoFile = 1;
    int totalHtmlFiles = m_veinDumps.count() + adjustmentHtmls + changeInfoFile;
    QDir htmlDir(htmlOutPath);
    htmlDir.setFilter(QDir::Files);
    QCOMPARE(htmlDir.count(), totalHtmlFiles);

    QTest::addColumn<QString>("htmlFilePath");
    for(auto &fileInfo: htmlDir.entryInfoList())
        QTest::newRow(fileInfo.fileName().toLatin1()) << fileInfo.absoluteFilePath();
}

void test_modman_regression_all_sessions::testGenerateScpiDocs()
{
    QFETCH(QString, htmlFilePath);
    QVERIFY(QFileInfo(htmlFilePath).size() > 0);
}

void test_modman_regression_all_sessions::testGenerateSnapshots_data()
{
    const QStringList expectedSnapshotsList = QDir(":/snapshots/").entryList(QStringList({"*.json"}));
    QTest::addColumn<QString>("snapshotName");
    for (const QString &snapshot: expectedSnapshotsList)
        QTest::newRow(snapshot.toLatin1()) << snapshot;
}

void test_modman_regression_all_sessions::testGenerateSnapshots()
{
    QStringList dumpedSnapshotsList = QDir(m_snapshotJsonsPath).entryList(QStringList({"*.json"}));
    QStringList expectedSnapshotsList = QDir(":/snapshots/").entryList(QStringList({"*.json"}));
    QCOMPARE(expectedSnapshotsList.count()+SessionExportGenerator::getBigSessionFileCount(), dumpedSnapshotsList.count());

    QFETCH(QString, snapshotName);
    QByteArray jsonExpected = TestLogHelpers::loadFile(QString(":/snapshots/%1").arg(snapshotName));
    QByteArray jsonDumped = TestLogHelpers::loadFile(QString(m_snapshotJsonsPath + "%1").arg(snapshotName));
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_modman_regression_all_sessions::checkObjectsProperlyDeleted()
{
    TestModuleManager::TModuleInstances cumulatedInstanceCounts = m_instanceCountsOnModulesDestroyed.last();

    QCOMPARE(cumulatedInstanceCounts.m_veinMetaDataCount, 0);
    QCOMPARE(cumulatedInstanceCounts.m_veinComponentCount, 0);
    QCOMPARE(cumulatedInstanceCounts.m_activistCount, 0);
    QCOMPARE(cumulatedInstanceCounts.m_serviceInterfaceCount, 0);
    QCOMPARE(cumulatedInstanceCounts.m_dspVarCount, 0);
    // 1. license, 2. Storage, 3.SystemModule, 4.Introspection, 5.VfLogger, 6. VfCmdEventHandlerSystemPtr(for VfClientRPCInvoker)
    constexpr int eventSystemCount = 6;
    QCOMPARE(cumulatedInstanceCounts.m_veinEventSystemCount, eventSystemCount);

    constexpr int serverPeers = 8;
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

    const QStringList sessionFileNames = m_veinDumps.keys();
    for(const QString &sessionFileName : sessionFileNames) {
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
