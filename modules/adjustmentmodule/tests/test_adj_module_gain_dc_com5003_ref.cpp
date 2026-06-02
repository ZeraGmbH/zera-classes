#include "test_adj_module_gain_dc_com5003_ref.h"
#include <timerfactoryqtfortest.h>
#include <QTest>

QTEST_MAIN(test_adj_module_gain_dc_com5003_ref)


void test_adj_module_gain_dc_com5003_ref::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_adj_module_gain_dc_com5003_ref::initTestCase_data()
{
    QTest::addColumn<QString>("refChannel");

    QTest::newRow("REF1") << "REF1";
    QTest::newRow("REF2") << "REF2";
    QTest::newRow("REF3") << "REF3";
    QTest::newRow("REF4") << "REF4";
    QTest::newRow("REF5") << "REF5";
    QTest::newRow("REF6") << "REF6";
}

void test_adj_module_gain_dc_com5003_ref::init()
{
    if(!m_testRunner)
        m_testRunner = std::make_unique<ModuleManagerTestRunner>("com5003-ref-session.json", true, "com5003");
    if(!m_scpiClient)
        m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
    QFETCH_GLOBAL(QString, refChannel);
    m_refChannel = refChannel;
    QCOMPARE(setRangeGroupingOff(), "+0");
    QCOMPARE(setAutoRangeOff(), "+0");
}

void test_adj_module_gain_dc_com5003_ref::cleanup()
{
    m_scpiClient.reset();
}

void test_adj_module_gain_dc_com5003_ref::checkIntialRangeR10V()
{
    QByteArray send = QString("*CLS|SENSE:RNG1:%1:RANGE?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "R10V");
}

void test_adj_module_gain_dc_com5003_ref::init480VRejected()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:INIT %1,480V;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::initR0VOk()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:INIT %1,R0V;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+0");
}

void test_adj_module_gain_dc_com5003_ref::initR10VOk()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:INIT %1,R10V;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+0");
}

void test_adj_module_gain_dc_com5003_ref::offsetStatusSet480VRejected()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:OSTATUS %1,480V,128;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::offsetStatusSetR0VOk()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:OSTATUS %1,R0V,128;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+0");
}

void test_adj_module_gain_dc_com5003_ref::offsetStatusSetR10VOk()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:OSTATUS %1,R10V,128;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+0");
}

void test_adj_module_gain_dc_com5003_ref::gainStatusSet480VRejected()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:GSTATUS %1,480V,128;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::gainStatusSetR0VOk()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:GSTATUS %1,R0V,128;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+0");
}

void test_adj_module_gain_dc_com5003_ref::gainStatusSetR10VOk()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:GSTATUS %1,R10V,128;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+0");
}

void test_adj_module_gain_dc_com5003_ref::phaseStatusSet480VRejected()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:PSTATUS %1,480V,128;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::phaseStatusSetR0VOk()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:PSTATUS %1,R0V,128;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+0");
}

void test_adj_module_gain_dc_com5003_ref::phaseStatusSetR10VOk()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:PSTATUS %1,R10V,128;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+0");
}

void test_adj_module_gain_dc_com5003_ref::offsetAdj480VRejected()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:OFFS %1,480V,0;|*STB?").arg(m_refChannel).toLatin1(); // 0: default actual value
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::offsetAdjR0VRejected()
{
    QCOMPARE(setRange(m_refChannel, "R0V"), "+0");
    QByteArray send = QString("*CLS|CALC:ADJ1:OFFS %1,R0V,0;|*STB?").arg(m_refChannel).toLatin1(); // 0: default actual value
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::offsetAdjR10VRejected()
{
    QCOMPARE(setRange(m_refChannel, "R10V"), "+0");
    QByteArray send = QString("*CLS|CALC:ADJ1:OFFS %1,R10V,0;|*STB?").arg(m_refChannel).toLatin1(); // 0: default actual value
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::gainAdj480VRejected()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:AMPL %1,480V,0;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::gainAdjR0VRejected()
{
    QCOMPARE(setRange(m_refChannel, "R0V"), "+0");
    QByteArray send = QString("*CLS|CALC:ADJ1:AMPL %1,R0V,0;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::gainAdjR10VOk()
{
    QCOMPARE(setRange(m_refChannel, "R10V"), "+0");
    QByteArray send = QString("*CLS|CALC:ADJ1:AMPL %1,R10V,0;|*STB?").arg(m_refChannel).toLatin1(); // 0: default actual value
    QCOMPARE(m_scpiClient->sendReceive(send), "+0");
}

void test_adj_module_gain_dc_com5003_ref::phaseAdj480VRejected()
{
    QByteArray send = QString("*CLS|CALC:ADJ1:PHAS %1,480V,0;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::phaseAdjR0VRejected()
{
    QCOMPARE(setRange(m_refChannel, "R0V"), "+0");
    QByteArray send = QString("*CLS|CALC:ADJ1:PHAS %1,R0V,0;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}

void test_adj_module_gain_dc_com5003_ref::phaseAdjR10VRejected()
{
    QCOMPARE(setRange(m_refChannel, "R10V"), "+0");
    QByteArray send = QString("*CLS|CALC:ADJ1:PHAS %1,R10V,0;|*STB?").arg(m_refChannel).toLatin1();
    QCOMPARE(m_scpiClient->sendReceive(send), "+4");
}


QString test_adj_module_gain_dc_com5003_ref::setAutoRangeOff()
{
    const QByteArray send = "CONFIGURATION:RNG1:RNGAUTO 0;|*stb?";
    return m_scpiClient->sendReceive(send);
}

QString test_adj_module_gain_dc_com5003_ref::setRangeGroupingOff()
{
    const QByteArray send = "CONFIGURATION:RNG1:GROUPING 0;|*stb?";
    return m_scpiClient->sendReceive(send);
}

QString test_adj_module_gain_dc_com5003_ref::setRange(const QString &channelAlias, const QString &rangeName)
{
    const QByteArray send = QString("SENSE:RNG1:%1:RANGE %2;|*stb?").arg(channelAlias, rangeName).toLatin1();
    return m_scpiClient->sendReceive(send);
}
