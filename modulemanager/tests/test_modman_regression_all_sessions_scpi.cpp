#include "test_modman_regression_all_sessions_scpi.h"
#include "demofactoryserviceinterfaces.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_modman_regression_all_sessions_scpi)

void test_modman_regression_all_sessions_scpi::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    TestModuleManager::enableTests();

    // TODO on lambdamodule: With fixed values lambdamodule causes empty SCPI return
    // TODO on SCPI: With fixed values SCPI can return just semicolons
    m_serviceInterfaceFactory = std::make_shared<DemoFactoryServiceInterfaces>(DemoFactoryServiceInterfaces::ReproducableChangeValues);
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());

    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), m_serviceInterfaceFactory);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
}

static constexpr int systemEntityId = 0;
static constexpr int valueWaitTime = 2000;

void test_modman_regression_all_sessions_scpi::init()
{
    QFETCH(QString, sessionFile);
    if (m_lastSessionFile != sessionFile) {
        m_lastSessionFile = sessionFile;

        m_modMan->destroyModulesAndWaitUntilAllShutdown();

        m_modMan->startAllTestServices(deduceDevice(sessionFile), false);
        m_modMan->changeSessionFile(sessionFile);
        m_modMan->waitUntilModulesAreReady();

        QCOMPARE(m_modmanFacade->getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session").toString(), sessionFile);
        TimeMachineForTest::getInstance()->processTimers(valueWaitTime);

        m_scpiModule = static_cast<SCPIMODULE::ScpiModuleForTest*>(m_modMan->getModule("scpimodule", 9999));
        QVERIFY(m_scpiModule != nullptr);

        m_scpiclient = new SCPIMODULE::ScpiTestClient(m_scpiModule,
                                                      *m_scpiModule->getConfigData(),
                                                      m_scpiModule->getScpiInterface());
        m_scpiModule->getSCPIServer()->appendClient(m_scpiclient); // deletes client
        connect(m_scpiclient, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, m_scpiclient, [&] (const QString &response) {
            m_scpiResponses.append(formatForDump(response));
        });
    }
}

void test_modman_regression_all_sessions_scpi::cleanup()
{
    m_scpiResponses.clear();
}

static const char* noTest = "Setup / No test";

void test_modman_regression_all_sessions_scpi::testScpi_data()
{
    // * This test takes long time
    // * Attempts to add SCPI dump to test_modman_regression_all_sessions did not work
    //   out nicely. Maybe we try again later...
    //   Remember that test_modman_regression_all_sessions is optimized for low time
    //   consumpton by switching sessions once and performing multiple operations on
    //   active session.
    // => So for now reduce time by ignoring few
    static const QStringList sessionToIgnore = QStringList() <<
                                               "mt310s2-meas-session-dev.json" <<
                                               "mt310s2-ced-session.json";

    const QString sessionPath = m_modMan->getInstalledSessionPath();
    const QStringList sessionList = QDir(sessionPath).entryList(QStringList({"*.json"}));
    QTest::addColumn<QString>("sessionFile");
    QTest::addColumn<QString>("scpiCmd");
    for (const QString &session : sessionList) {
        if (sessionToIgnore.contains(session))
            continue;
        const QStringList scpiCmds{noTest, "READ?", "FETCH?", "MEASURE?"};
        for (const QString &scpiCmd : scpiCmds)
            QTest::newRow(session.toLatin1() + ": " + scpiCmd.toLatin1()) << session << scpiCmd;
    }
}

void test_modman_regression_all_sessions_scpi::testScpi()
{
    QFETCH(QString, scpiCmd);
    if(scpiCmd == noTest)
        return;
    m_scpiclient->sendScpiCmds(scpiCmd);
    TimeMachineForTest::getInstance()->processTimers(valueWaitTime);

    QCOMPARE(m_scpiResponses.count(), 1);
    QString dumped = m_scpiResponses[0];

    QFETCH(QString, sessionFile);
    const QString expectedFilePath = genFilePath(sessionFile, scpiCmd);
    qInfo("File with expected: %s", qPrintable(expectedFilePath));
    QByteArray expected = TestLogHelpers::loadFile(expectedFilePath);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
}

QString test_modman_regression_all_sessions_scpi::deduceDevice(const QString &sessionFileName)
{
    QStringList sessionFileParts = sessionFileName.split("-");
    return sessionFileParts[0];
}

QString test_modman_regression_all_sessions_scpi::formatForDump(const QString &scpiResponse)
{
    QString inLineSeparated = scpiResponse;
    inLineSeparated = inLineSeparated.replace(";", ";\n");
    const QStringList inLines = inLineSeparated.split("\n");

    // TODO SCPIModule: In case of irregularly changing values we see sequences of ';'
    QMap<QString, QStringList> sorted;
    for (const QString &line : inLines)
        sorted[line].append(line);

    QStringList outLines;
    for (const QStringList &entry : sorted)
        outLines.append(entry);
    Q_ASSERT(inLines.size() == outLines.size());
    return outLines.join("\n");
}

QString test_modman_regression_all_sessions_scpi::genFilePath(QString sessionFile, const QString &scpiCmd)
{
    const QString expectedFileName = sessionFile.replace(".json", "");
    QString subPath = "other";
    if (scpiCmd == "MEASURE?")
        subPath = "measure";
    return QString(":/scpiDumps/%1/%2").arg(subPath, expectedFileName);
}
