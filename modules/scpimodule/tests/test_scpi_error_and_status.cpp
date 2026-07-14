#include "test_scpi_error_and_status.h"
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
    QTest::addColumn<SortTypes>("sortType");
    QTest::newRow("Sorted") << SORTED;
    QTest::newRow("Not-Sorted") << NOT_SORTED;
}

void test_scpi_error_and_status::sendBullshitQueryStb()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "FOO:BAR?"), "");
    QCOMPARE(sendReceive(client, "*STB?"), "+4");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::sendBullshitTwiceQueryErrorCount()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "FOO:BAR?"),"");
    QCOMPARE(sendReceive(client, "STATUS:QUESTIONABLE:ENABLE FOO;"),"");

    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT?"), "2");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:ALL?"), "-100,Command error;-120,Numeric data error");

    QCOMPARE(client.getHandledResponses(), 4);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::sendValidCommandWithoutSemicolonQueryStb()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "CONFIGURATION:RNG1:GROUPING 1"), "");
    QCOMPARE(sendReceive(client, "*STB?"), "+4"); // 1<<STBeeQueueNotEmpty
    QCOMPARE(sendReceive(client, "*ESR?"), "+0");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");
    QCOMPARE(sendReceive(client, "*STB?"), "+0");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT?"), "0");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "+0,No error");

    QCOMPARE(client.getHandledResponses(), 8);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::overflowErrorQueue()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    const int errorQueueLen = SCPIMODULE::cIEEE4882::getErrorQueueLength();
    for (int i=0; i<SCPIMODULE::cIEEE4882::getErrorQueueLength(); ++i)
        sendReceive(client, "FOO:BAR?");

    const QString expected = QString("%1").arg(errorQueueLen);
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT?"), expected);
    sendReceive(client, "FOO:BAR?");
    QTest::qWait(200); // WTF???
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT?"), expected);
    for (int i=0; i<SCPIMODULE::cIEEE4882::getErrorQueueLength()-1; ++i)
        QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-350,Queue overflow");

    QCOMPARE(client.getHandledResponses(), 2*SCPIMODULE::cIEEE4882::getErrorQueueLength() + 3);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeErrorAndClearIt()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "FOO:BAR?"),"");

    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(sendReceive(client, "*CLS"),"");
    QCOMPARE(sendReceive(client, "*STB?"), "+0");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "+0,No error");

    QCOMPARE(client.getHandledResponses(), 5);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::noErrorCrossTalkOnMultipleClients()
{
    SCPIMODULE::ScpiTestClient client1(getScpiModule());
    SCPIMODULE::ScpiTestClient client2(getScpiModule());

    QCOMPARE(sendReceive(client1, "FOO:BAR?"),"");
    QCOMPARE(sendReceive(client1, "SYSTEM:ERROR:COUNT?"), "1");
    QCOMPARE(sendReceive(client2, "SYSTEM:ERROR:COUNT?"), "0");
}

void test_scpi_error_and_status::noQuestionableEnableCrossTalkOnMultipleClients()
{
    SCPIMODULE::ScpiTestClient client1(getScpiModule());
    SCPIMODULE::ScpiTestClient client2(getScpiModule());

    QCOMPARE(sendReceive(client1, "STATUS:QUESTIONABLE:ENABLE?"), "0");
    QCOMPARE(sendReceive(client2, "STATUS:QUESTIONABLE:ENABLE?"), "0");
    QCOMPARE(sendReceive(client1, "SYSTEM:ERROR:COUNT?"), "0");
    QCOMPARE(sendReceive(client2, "SYSTEM:ERROR:COUNT?"), "0");

    QCOMPARE(sendReceive(client1, "STATUS:QUESTIONABLE:ENABLE 1;"), "");
    QCOMPARE(sendReceive(client1, "SYSTEM:ERROR:COUNT?"), "0");

    QCOMPARE(sendReceive(client1, "STATUS:QUESTIONABLE:ENABLE?"), "1");
    QCOMPARE(sendReceive(client2, "STATUS:QUESTIONABLE:ENABLE?"), "0");
}

void test_scpi_error_and_status::causeCommandErrorOpc()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*OPC 1;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorEse()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*ESE 1;2;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorSre()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*SRE 1;2;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorCls()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*CLS 1;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeQueryErrorCls()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*CLS?"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-400,Query error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorRst()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*RST 1;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeQueryErrorRst()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*RST?"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-400,Query error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorIdn()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*IDN;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorEsr()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*ESR;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorStb()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*STB;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorTst()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "*TST;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorReadError()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:ERROR;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorReadErrorCount()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:COUNT;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::causeCommandErrorReadAllErrors()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "SYSTEM:ERROR:ALL;"),"");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-100,Command error");

    QCOMPARE(client.getHandledResponses(), 2);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_error_and_status::sendStatusQuestionableEnableInvalidParam()
{
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QCOMPARE(sendReceive(client, "STATUS:QUESTIONABLE:ENABLE FOO;"), "");

    QCOMPARE(sendReceive(client, "*STB?"), "+4"); // 1<<STBeeQueueNotEmpty
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "-120,Numeric data error");
    QCOMPARE(sendReceive(client, "*STB?"), "+0");
    QCOMPARE(sendReceive(client, "SYSTEM:ERROR?"), "+0,No error");

    QCOMPARE(client.getHandledResponses(), 5);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

SCPIMODULE::cSCPIModule *test_scpi_error_and_status::getScpiModule()
{
    return qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
}

QString test_scpi_error_and_status::sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi)
{
    QFETCH_GLOBAL(SortTypes, sortType);
    switch (sortType) {
    case SORTED:
        return client.sendReceiveSorted(scpi, true);
    case NOT_SORTED:
        return client.sendReceiveNotSorted(scpi, true);
    }
}
