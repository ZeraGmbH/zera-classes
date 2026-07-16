#include "test_adj_data.h"
#include "scpitestclient.h"
#include <timerfactoryqtfortest.h>
#include <testloghelpers.h>
#include <mockeepromdevice.h>
#include <mt310s2systeminfomock.h>
#include <mockserverparamgenerator.h>
#include <QTest>

QTEST_MAIN(test_adj_data)

void test_adj_data::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_adj_data::init()
{
    Mt310s2SystemInfoMock::setSerialNumber("050059467");
    Mt310s2SystemInfoMock::setDeviceName("MT310s2 ADW5859");
}

void test_adj_data::cleanup()
{
    m_testRunner = nullptr;
}

void test_adj_data::resetAdjustmentDeviceNotAdjusted()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json", true);
    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QString response = client.sendReceiveNotSorted("STATUS:DEV1:ADJUSTMENT?", true);
    QCOMPARE(response, "5"); //Device not adjusted + Wrong serial number
    response = client.sendReceiveNotSorted("RESET:DEV1:ADJUSTMENT;", true);
    QCOMPARE(response, "");
    response = client.sendReceiveNotSorted("STATUS:DEV1:ADJUSTMENT?", true);
    QCOMPARE(response, "5");
}

void test_adj_data::resetAdjustmentDeviceAdjusted()
{
    prepareEepromTakenFromAdjustedDevice();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json", true);

    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QString response = client.sendReceiveNotSorted("STATUS:DEV1:ADJUSTMENT?", true);
    QCOMPARE(response, "0"); //Device adjusted
    response = client.sendReceiveNotSorted("RESET:DEV1:ADJUSTMENT;", true);
    QCOMPARE(response, "");
    response = client.sendReceiveNotSorted("STATUS:DEV1:ADJUSTMENT?", true);
    QCOMPARE(response, "1"); //Device Not adjusted
}

void test_adj_data::resetAdjustmentTwice()
{
    prepareEepromTakenFromAdjustedDevice();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json", true);

    SCPIMODULE::ScpiTestClient client(getScpiModule());

    QString response = client.sendReceiveNotSorted("STATUS:DEV1:ADJUSTMENT?", true);
    QCOMPARE(response, "0"); //Device adjusted
    client.sendReceiveNotSorted("RESET:DEV1:ADJUSTMENT;", true);
    client.sendReceiveNotSorted("RESET:DEV1:ADJUSTMENT;", true);
    response = client.sendReceiveNotSorted("STATUS:DEV1:ADJUSTMENT?", true);
    QCOMPARE(response, "1"); //Device Not adjusted
}

void test_adj_data::prepareEepromTakenFromAdjustedDevice()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("mt310s2d"));
    I2cSettingsPtr i2cSettings = settings->getI2cSettings();
    MockEepromDevice::setData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
                              TestLogHelpers::loadFile(":/mt310s2-050059467.eeprom"));
}

SCPIMODULE::cSCPIModule *test_adj_data::getScpiModule()
{
    return qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
}
