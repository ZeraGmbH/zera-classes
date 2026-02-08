#include "test_recorder_scpi.h"
#include "scpimoduleclientblocked.h"
#include <timerfactoryqtfortest.h>
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <vf-cpp-entity.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_recorder_scpi)

constexpr int rmsEntityId = 1040;
constexpr int powerEntityId = 1070;
static constexpr int sigMeasuringEntityId = 1050; // DftModule

void test_recorder_scpi::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_recorder_scpi::init()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/mt310s2-emob-session-ac-scpi.json");
}

void test_recorder_scpi::cleanup()
{
    m_testRunner = nullptr;
}

void test_recorder_scpi::scpiQueryRunInitial()
{
    ScpiModuleClientBlocked client;
    QCOMPARE(client.sendReceive("RECORDER:REC1:RUN?"), "0");
}

void test_recorder_scpi::scpiQueryCountEmpty()
{
    ScpiModuleClientBlocked client;
    QCOMPARE(client.sendReceive("RECORDER:REC1:COUNT?"), "0");
}

void test_recorder_scpi::scpiWriteCountIgnored()
{
    ScpiModuleClientBlocked client;
    QCOMPARE(client.sendReceive("*cls|RECORDER:REC1:COUNT 1;|*stb?"), "+4");
    QCOMPARE(client.sendReceive("*cls|RECORDER:REC1:COUNT?"), "0");
}

void test_recorder_scpi::scpiQueryJsonExportEmpty()
{
    ScpiModuleClientBlocked client;
    QString receive = client.sendReceive("RECORDER:REC1:EXPORT:JSON?", false);
    QCOMPARE(receive, "{\n"
                      "}\n"
                      "\n");
    QCOMPARE(client.sendReceive("*OPC?"), "+1");
}

void test_recorder_scpi::scpiQueryJsonFireValuesOnce()
{
    createModulesManually();
    TimeMachineForTest::getInstance()->setCurrentTime(QDate(2026, 2, 8),
                                                      QTime(17, 21, 37, 1),
                                                      QTimeZone(3*3600));

    ScpiModuleClientBlocked client;
    client.sendReceive("RECORDER:REC1:RUN 1;");

    fireActualValues();
    triggerDftModuleSigMeasuring();
    QString jsonDumped = client.sendReceive("RECORDER:REC1:EXPORT:JSON?", false);
    QString jsonExpected = TestLogHelpers::loadFile(":/fireValuesOnce.json");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_recorder_scpi::scpiQueryJsonFireValuesTwice()
{
    createModulesManually();
    TimeMachineForTest::getInstance()->setCurrentTime(QDate(2026, 2, 8),
                                                      QTime(8, 2, 59, 999),
                                                      QTimeZone(1*3600));

    ScpiModuleClientBlocked client;
    client.sendReceive("RECORDER:REC1:RUN 1;");

    fireActualValues();
    triggerDftModuleSigMeasuring();
    TimeMachineForTest::getInstance()->processTimers(499);
    triggerDftModuleSigMeasuring();
    QString jsonDumped = client.sendReceive("RECORDER:REC1:EXPORT:JSON?", false);
    QString jsonExpected = TestLogHelpers::loadFile(":/fireValuesTwice.json");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_recorder_scpi::scpiQueryAll()
{
    ScpiModuleClientBlocked client;
    QCOMPARE(client.sendReceive("*cls|RECORDER|*stb?"), "+4");
    QCOMPARE(client.sendReceive("*cls|RECORDER:REC1|*stb?"), "+4");
}

void test_recorder_scpi::createModule(int entityId, QMap<QString, QVariant> components)
{
    VfCpp::VfCppEntity *entity = new VfCpp::VfCppEntity(entityId);
    m_testRunner->getModManFacade()->addSubsystem(entity);
    entity->initModule();
    for(const auto &compoName : components.keys())
        entity->createComponent(compoName, components[compoName]);
    TimeMachineObject::feedEventLoop();
}

void test_recorder_scpi::fireActualValues()
{
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN1", 1.1);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN2", 1.0+(1.0/1e6));
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN3", 3.333333e5);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN4", 4.45959595e5);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN5", 5.55555555595e-7);
    m_testRunner->setVfComponent(rmsEntityId, "ACT_RMSPN6", 6.67895678e-3);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS1", 1200.676767676);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS2", 13333337779.777888999);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS3", 0.0000000123456789);
    m_testRunner->setVfComponent(powerEntityId, "ACT_PQS4", 0.100000565656);
}

void test_recorder_scpi::triggerDftModuleSigMeasuring()
{
    m_testRunner->setVfComponent(sigMeasuringEntityId, "SIG_Measuring", QVariant(0));
    m_testRunner->setVfComponent(sigMeasuringEntityId, "SIG_Measuring", QVariant(1));
}

void test_recorder_scpi::createModulesManually()
{
    QVariantMap components = {
        {"ACT_RMSPN1", QVariant()},
        {"ACT_RMSPN2", QVariant()},
        {"ACT_RMSPN3", QVariant()},
        {"ACT_RMSPN4", QVariant()},
        {"ACT_RMSPN5", QVariant()},
        {"ACT_RMSPN6", QVariant()}
    };
    createModule(rmsEntityId, components);
    components = {{"SIG_Measuring", QVariant(1)}};
    createModule(sigMeasuringEntityId, components);
    components = {
        {"ACT_PQS1", QVariant()},
        {"ACT_PQS2", QVariant()},
        {"ACT_PQS3", QVariant()},
        {"ACT_PQS4", QVariant()}
    };
    createModule(powerEntityId, components);
}
