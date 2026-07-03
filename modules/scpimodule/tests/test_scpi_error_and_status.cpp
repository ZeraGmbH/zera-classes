#include "test_scpi_error_and_status.h"
#include "scpitestselectableclient.h"
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

void test_scpi_error_and_status::initTestCase_data()
{
    QTest::addColumn<ScpiTestSelectableClient::ClientType>("clientType");
    QTest::newRow("Test") << ScpiTestSelectableClient::TEST;
    QTest::newRow("Net") << ScpiTestSelectableClient::NET;
}

void test_scpi_error_and_status::sendBullshitQueryStb()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("FOO:BAR?"),"");
    QCOMPARE(client.sendReceive("*STB?"), "+4");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::sendBullshitTwiceQueryErrorCount()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("FOO:BAR?"),"");
    QCOMPARE(client.sendReceive("STATUS:QUESTIONABLE:ENABLE FOO;"),"");

    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), "2");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR:ALL?"), "-100,Command error;-120,Numeric data error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::sendValidCommandWithoutSemicolonQueryStb()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:GROUPING 1"), "");

    QCOMPARE(client.sendReceive("*STB?"), "+4"); // 1<<STBeeQueueNotEmpty
    QCOMPARE(client.sendReceive("*ESR?"), "+0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QCOMPARE(client.sendReceive("*STB?"), "+0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), "0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "+0,No error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::overflowErrorQueue()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

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
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeErrorAndClearIt()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("FOO:BAR?"),"");

    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(client.sendReceive("*CLS"),"");
    QTest::qWait(200); // WTF???
    QCOMPARE(client.sendReceive("*STB?"), "+0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "+0,No error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorOpc()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*OPC 1;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorEse()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*ESE 1;2;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorSre()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*SRE 1;2;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorCls()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*CLS 1;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeQueryErrorCls()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*CLS?"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-400,Query error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorRst()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*RST 1;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeQueryErrorRst()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*RST?"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-400,Query error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorIdn()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*IDN;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorEsr()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*ESR;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorStb()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*STB;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorTst()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("*TST;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorReadError()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("SYSTEM:ERROR;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorReadErrorCount()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("SYSTEM:ERROR:COUNT;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::causeCommandErrorReadAllErrors()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("SYSTEM:ERROR:ALL;"),"");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-100,Command error");
    QVERIFY(!client.commandsPending());
}

void test_scpi_error_and_status::sendStatusQuestionableEnableInvalidParam()
{
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("STATUS:QUESTIONABLE:ENABLE FOO;"), "");

    QCOMPARE(client.sendReceive("*STB?"), "+4"); // 1<<STBeeQueueNotEmpty
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "-120,Numeric data error");
    QCOMPARE(client.sendReceive("*STB?"), "+0");
    QCOMPARE(client.sendReceive("SYSTEM:ERROR?"), "+0,No error");
    QVERIFY(!client.commandsPending());
}

SCPIMODULE::cSCPIModule *test_scpi_error_and_status::getScpiModule()
{
    return static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
}
