#include "test_modman_regression_all_sessions_scpi.h"
#include "demofactoryserviceinterfaces.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_modman_regression_all_sessions_scpi)

void test_modman_regression_all_sessions_scpi::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();

    // TODO: Once Lambdamodule mystery on fixed values is solved we have to move to fixed values
    // to allow dump checks.
    m_serviceInterfaceFactory = std::make_shared<DemoFactoryServiceInterfaces>(DemoFactoryServiceInterfaces::RandomValues);
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());

    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), m_serviceInterfaceFactory);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
}

void test_modman_regression_all_sessions_scpi::initTestCase_data()
{
    const QString sessionPath = m_modMan->getInstalledSessionPath();
    const QStringList sessionList = QDir(sessionPath).entryList(QStringList({"*.json"}));
    QTest::addColumn<QString>("device");
    QTest::addColumn<QString>("sessionFile");

    // * This test takes long time
    // * Attempts to add SCPI dump to test_modman_regression_all_sessions did not work
    //   out nicely. Maybe we try again later...
    //   Remember that test_modman_regression_all_sessions is optimized for low time
    //   consumpton by switching sessions once and performing multiple operations on
    //   active session.
    // => So for now reduce time by ignoring few sessions
    static const QStringList sessionToIgnore = QStringList() <<
                                               "com5003-meas-session.json" <<
                                               "mt310s2-meas-session-dev.json";

    // QTest::newRow requires char* living till end of test. So keep texts in m_testRowData.
    for (const QString &session : sessionList) {
        if (sessionToIgnore.contains(session))
            continue;
        const QByteArray device = deduceDevice(session);
        const QByteArray testTitle = device.toUpper()+ " / " + session.toLatin1();
        m_testRowData.append({testTitle, device, session});
    }
    for (const TestRow row : m_testRowData)
        QTest::newRow(row.testName.constData()) << row.deviceName << row.sessionFile;
}

static constexpr int systemEntityId = 0;
static constexpr int valueWaitTime = 2000;

void test_modman_regression_all_sessions_scpi::init()
{
    QFETCH_GLOBAL(QString, device);
    m_modMan->startAllTestServices(device, false);
    QFETCH_GLOBAL(QString, sessionFile);
    m_modMan->changeSessionFile(sessionFile);
    m_modMan->waitUntilModulesAreReady();

    QCOMPARE(m_modmanFacade->getStorageSystem()->getDb()->getStoredValue(systemEntityId, "Session").toString(), sessionFile);
    TimeMachineObject::feedEventLoop();
    TimeMachineForTest::getInstance()->processTimers(valueWaitTime);

    m_scpiModule = static_cast<SCPIMODULE::ScpiModuleForTest*>(m_modMan->getModule("scpimodule", 9999));
    QVERIFY(m_scpiModule != nullptr);

    m_scpiclient = new SCPIMODULE::ScpiTestClient(m_scpiModule,
                                                  *m_scpiModule->getConfigData(),
                                                  m_scpiModule->getScpiInterface());
    m_scpiModule->getSCPIServer()->appendClient(m_scpiclient); // deletes client
    connect(m_scpiclient, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, m_scpiclient, [&] (QString response) {
        m_scpiResponse = response.replace(";", ";\n");
    });
}

void test_modman_regression_all_sessions_scpi::cleanup()
{
    m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_scpiResponse.clear();
}

void test_modman_regression_all_sessions_scpi::testScpiMeasure()
{
    m_scpiclient->sendScpiCmds("MEASURE?");
    TimeMachineForTest::getInstance()->processTimers(2000);

    qWarning("%s", qPrintable(m_scpiResponse));
    QVERIFY(!m_scpiResponse.isEmpty());
}

QByteArray test_modman_regression_all_sessions_scpi::deduceDevice(const QString &sessionFileName)
{
    QStringList sessionFileParts = sessionFileName.split("-");
    return sessionFileParts[0].toLatin1();
}
