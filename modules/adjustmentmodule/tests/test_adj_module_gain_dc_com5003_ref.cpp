#include "test_adj_module_gain_dc_com5003_ref.h"
#include <timerfactoryqtfortest.h>
#include <QTest>

QTEST_MAIN(test_adj_module_gain_dc_com5003_ref)


void test_adj_module_gain_dc_com5003_ref::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_adj_module_gain_dc_com5003_ref::init()
{
    if(!m_testRunner)
        m_testRunner = std::make_unique<ModuleManagerTestRunner>("com5003-ref-session.json", true, "com5003");
    if(!m_scpiClient)
        m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
}

void test_adj_module_gain_dc_com5003_ref::cleanup()
{
    m_scpiClient.reset();
}

void test_adj_module_gain_dc_com5003_ref::checkIntialRangeR10V()
{
    QString response;
    response = m_scpiClient->sendReceive("SENSE:RNG1:REF1:RANGE?");
    QCOMPARE(response, "R10V");
    response = m_scpiClient->sendReceive("SENSE:RNG1:REF2:RANGE?");
    QCOMPARE(response, "R10V");
    response = m_scpiClient->sendReceive("SENSE:RNG1:REF3:RANGE?");
    QCOMPARE(response, "R10V");
    response = m_scpiClient->sendReceive("SENSE:RNG1:REF4:RANGE?");
    QCOMPARE(response, "R10V");
    response = m_scpiClient->sendReceive("SENSE:RNG1:REF5:RANGE?");
    QCOMPARE(response, "R10V");
    response = m_scpiClient->sendReceive("SENSE:RNG1:REF6:RANGE?");
    QCOMPARE(response, "R10V");
}

void test_adj_module_gain_dc_com5003_ref::init480VNotOk()
{
    QString response;
    response = m_scpiClient->sendReceive("CALC:ADJ1:INIT REF1,480V;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc_com5003_ref::initR10VOk()
{
    QString response;
    response = m_scpiClient->sendReceive("CALC:ADJ1:INIT REF1,R10V;|*stb?");
    QCOMPARE(response, "+0");
    response = m_scpiClient->sendReceive("CALC:ADJ1:INIT REF2,R10V;|*stb?");
    QCOMPARE(response, "+0");
    response = m_scpiClient->sendReceive("CALC:ADJ1:INIT REF3,R10V;|*stb?");
    QCOMPARE(response, "+0");
    response = m_scpiClient->sendReceive("CALC:ADJ1:INIT REF4,R10V;|*stb?");
    QCOMPARE(response, "+0");
    response = m_scpiClient->sendReceive("CALC:ADJ1:INIT REF5,R10V;|*stb?");
    QCOMPARE(response, "+0");
    response = m_scpiClient->sendReceive("CALC:ADJ1:INIT REF6,R10V;|*stb?");
    QCOMPARE(response, "+0");
}
