#include "test_read_clear_emob_error.h"
#include "controllerpersitentdata.h"
#include <vn_networksystem.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_read_clear_emob_error)

static int constexpr hotplugControlsEntityId = 1700;
static int constexpr serverPort = 4711;
static constexpr int systemEntityId = 0;

void test_read_clear_emob_error::init()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/hotpluscontrols-min-session.json");
    m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
}

void test_read_clear_emob_error::cleanup()
{
    m_testRunner.reset();
    m_scpiClient.reset();
    ControllerPersitentData::cleanupPersitentData();
}

void test_read_clear_emob_error::readClearErrorNoHotplugScpi()
{
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:ERROR?");
    QCOMPARE(status, ""); //no emob

    status = m_scpiClient->sendReceive("EMOB:HOTP1:CLEARERROR;|*stb?");
    QCOMPARE(status, "+4");
}

void test_read_clear_emob_error::readClearErrorNoParamOneEmobScpi()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    m_testRunner->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:ERROR?");
    QCOMPARE(status, QString::number(errorInstrumentStatus::Instrument_Status_Cable_Error));

    status = m_scpiClient->sendReceive("EMOB:HOTP1:CLEARERROR;|*stb?");
    QCOMPARE(status, "+0");
}

void test_read_clear_emob_error::readClearErrorNoParamOneMt650eScpi()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL2", {"Mt650e", cClamp::undefined});
    m_testRunner->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:ERROR?");
    QCOMPARE(status, QString::number(errorInstrumentStatus::Instrument_Status_Cable_Error));

    status = m_scpiClient->sendReceive("EMOB:HOTP1:CLEARERROR;|*stb?");
    QCOMPARE(status, "+0");
}

void test_read_clear_emob_error::readClearErrorInvalidParamOneEmobOneMt650eScpi()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    infoMap.insert("IL2", {"Mt650e", cClamp::undefined});
    m_testRunner->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();


    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:ERROR? FOO;");
    QCOMPARE(status, "");

    status = m_scpiClient->sendReceive("EMOB:HOTP1:CLEARERROR FOO;|*stb?");
    QCOMPARE(status, "+4");
}

void test_read_clear_emob_error::readClearErrorInCorrectParamOneEmobOneMt650eScpi()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    infoMap.insert("IL2", {"Mt650e", cClamp::undefined});
    m_testRunner->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:ERROR? IL3;");
    QCOMPARE(status, "");

    status = m_scpiClient->sendReceive("EMOB:HOTP1:CLEARERROR IL3;|*stb?");
    QCOMPARE(status, "+4");
}

void test_read_clear_emob_error::readClearErrorValidParamOneEmobOneMt650eScpi()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB", cClamp::undefined});
    infoMap.insert("IL2", {"Mt650e", cClamp::undefined});
    m_testRunner->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:ERROR? IL1;");
    QCOMPARE(status, QString::number(errorInstrumentStatus::Instrument_Status_Cable_Error));
    status = m_scpiClient->sendReceive("EMOB:HOTP1:CLEARERROR  IL1;|*stb?");
    QCOMPARE(status, "+0");

    status = m_scpiClient->sendReceive("EMOB:HOTP1:ERROR? IL2;");
    QCOMPARE(status, QString::number(errorInstrumentStatus::Instrument_Status_Cable_Error));
    status = m_scpiClient->sendReceive("EMOB:HOTP1:CLEARERROR IL2;|*stb?");
    QCOMPARE(status, "+0");
}
