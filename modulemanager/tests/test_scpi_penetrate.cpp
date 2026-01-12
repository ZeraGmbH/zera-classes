#include "test_scpi_penetrate.h"
#include "demofactoryserviceinterfaces.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_scpi_penetrate)

void test_scpi_penetrate::initTestCase()
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

void test_scpi_penetrate::init()
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

        m_scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_modMan->getModule("scpimodule", 9999));
        QVERIFY(m_scpiModule != nullptr);

        createScpiClient();
    }
}

void test_scpi_penetrate::cleanup()
{
    m_scpiResponses.clear();
}

void test_scpi_penetrate::penetrateRangeSet_data()
{
    QTest::addColumn<QString>("sessionFile");
    QTest::addColumn<QStringList>("scpiCmds");
    const QStringList sessionList = getAvailableSessions();
    static const QStringList sessionToTest = QStringList() <<
                                             //"mt310s2-meas-session.json" <<
                                             "com5003-meas-session.json";
    for (const QString &session : sessionList) {
        if (sessionToTest.contains(session)) {
            const QStringList scpiCmds{"SENSE:RNG1:IL1:RANGE 10A;", "SENSE:RNG1:IL1:RANGE 5A;", "SENSE:RNG1:IL2:RANGE 10A;", "SENSE:RNG1:IL2:RANGE 5A;"};
            QTest::newRow(session.toLatin1() + ":") << session << scpiCmds;
        }
    }
}

void test_scpi_penetrate::penetrateRangeSet()
{
    QFETCH(QStringList, scpiCmds);
    for (const QString &scpiCmd : scpiCmds)
        m_scpiclient->sendScpiCmds(scpiCmd);
    m_scpiModule->getSCPIServer()->deleteSCPIClient(m_scpiclient);
    TimeMachineObject::feedEventLoop();

    createScpiClient();
    for (const QString &scpiCmd : scpiCmds)
        m_scpiclient->sendScpiCmds(scpiCmd);
    TimeMachineObject::feedEventLoop();
}

QStringList test_scpi_penetrate::getAvailableSessions()
{
    const QString sessionPath = m_modMan->getInstalledSessionPath();
    return QDir(sessionPath).entryList(QStringList({"*.json"}));
}

void test_scpi_penetrate::createScpiClient()
{
    m_scpiclient = new SCPIMODULE::ScpiTestClient(m_scpiModule,
                                                  *m_scpiModule->getConfData(),
                                                  m_scpiModule->getSCPIServer()->getScpiInterface());
    m_scpiModule->getSCPIServer()->appendClient(m_scpiclient);
    connect(m_scpiclient, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, m_scpiclient, [&] (const QString &response) {
        m_scpiResponses.append(formatForDump(response));
    });
}

QString test_scpi_penetrate::deduceDevice(const QString &sessionFileName)
{
    QStringList sessionFileParts = sessionFileName.split("-");
    return sessionFileParts[0];
}

QString test_scpi_penetrate::formatForDump(const QString &scpiResponse)
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

QString test_scpi_penetrate::genFilePath(QString sessionFile, const QString &scpiCmd)
{
    const QString expectedFileName = sessionFile.replace(".json", "");
    QString subPath = "other";
    if (scpiCmd == "MEASURE?")
        subPath = "measure";
    return QString(":/scpiDumps/%1/%2").arg(subPath, expectedFileName);
}
