#include "test_scpi_error_and_status.h"
#include "scpimodulenetclientblocked.h"
#include "ieee488-2.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <QTest>

QTEST_MAIN(test_scpi_error_and_status)

void test_scpi_error_and_status::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json");
}

void test_scpi_error_and_status::sendBullshitQueryStb()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("FOO:BAR?"),"");
    QCOMPARE(client.sendReceive("*STB?"), "+4");
}

void test_scpi_error_and_status::sendBullshitTwiceQueryErrorCount()
{
    ScpiModuleNetClientBlocked client;
    QCOMPARE(client.sendReceive("FOO:BAR?"),"");
    QCOMPARE(client.sendReceive("STATUS:QUESTIONABLE:ENABLE FOO;"),"");

    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), "2");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR:ALL?"), "-100,Command error;-120,Numeric data error");
}

void test_scpi_error_and_status::sendValidCommandWithoutSemicolonQueryStb()
{
    ScpiModuleNetClientBlocked client;
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:GROUPING 1"), "");

    QCOMPARE(client.sendReceive("*STB?"), "+4"); // 1<<STBeeQueueNotEmpty
    QCOMPARE(client.sendReceive("*ESR?"), "+0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QCOMPARE(client.sendReceive("*STB?"), "+0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), "0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "+0,No error");
}

void test_scpi_error_and_status::overflowErrorQueue()
{
    ScpiModuleNetClientBlocked client;
    const int errorQueueLen = SCPIMODULE::cIEEE4882::getErrorQueueLength();
    for (int i=0; i<SCPIMODULE::cIEEE4882::getErrorQueueLength(); ++i)
        client.sendReceive("FOO:BAR?");

    const QString expected = QString("%1").arg(errorQueueLen);
    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), expected);
    client.sendReceive("FOO:BAR?");
    QTest::qWait(200); // WTF???
    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), expected);
    for (int i=0; i<SCPIMODULE::cIEEE4882::getErrorQueueLength()-1; ++i)
        QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-350,Queue overflow");
}

void test_scpi_error_and_status::causeErrorAndClearIt()
{
    ScpiModuleNetClientBlocked client;
    QCOMPARE(client.sendReceive("FOO:BAR?"),"");

    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(client.sendReceive("*CLS"),"");
    QTest::qWait(200); // WTF???
    QCOMPARE(client.sendReceive("*STB?"), "+0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "+0,No error");
}

void test_scpi_error_and_status::causeCommandErrorOpc()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*OPC 1;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeCommandErrorEse()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*ESE 1;2;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeCommandErrorSre()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*SRE 1;2;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeCommandErrorCls()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*CLS 1;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeQueryErrorCls()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*CLS?"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-400,Query error");
}

void test_scpi_error_and_status::causeCommandErrorRst()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*RST 1;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeQueryErrorRst()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*RST?"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-400,Query error");
}

void test_scpi_error_and_status::causeCommandErrorIdn()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*IDN;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeCommandErrorEsr()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*ESR;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeCommandErrorStb()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*STB;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeCommandErrorTst()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("*TST;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeCommandErrorReadError()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("SYSTEM:ERROR;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeCommandErrorReadErrorCount()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::causeCommandErrorReadAllErrors()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("SYSTEM:ERROR:ALL;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
}

void test_scpi_error_and_status::sendStatusQuestionableEnableInvalidParam()
{
    ScpiModuleNetClientBlocked client;

    QCOMPARE(client.sendReceive("STATUS:QUESTIONABLE:ENABLE FOO;"), "");

    QCOMPARE(client.sendReceive("*STB?"), "+4"); // 1<<STBeeQueueNotEmpty
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-120,Numeric data error");
    QCOMPARE(client.sendReceive("*STB?"), "+0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "+0,No error");
}
