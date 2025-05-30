#include "blemodulemeasprogram.h"
#include "blemodule.h"
#include "blemoduleconfiguration.h"
#include <boolvalidator.h>
#include <regexvalidator.h>
#include <errormessages.h>
#include <reply.h>
#include <scpi.h>

namespace BLEMODULE
{

cBleModuleMeasProgram::cBleModuleMeasProgram(cBleModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
    m_bluetooth = BluetoothConvenienceFacade::getInstance();
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_activationDoneState);
    connect(&m_activationDoneState, &QState::entered, this, &cBleModuleMeasProgram::activateDone);

    m_deactivationMachine.addState(&m_deactivateDoneState);
    m_deactivationMachine.setInitialState(&m_deactivateDoneState);
    connect(&m_deactivateDoneState, &QState::entered, this, &cBleModuleMeasProgram::deactivateMeasDone);
}

cBleModuleMeasProgram::~cBleModuleMeasProgram()
{
    m_bluetooth->removeBleDecoder(m_bleDispatcherId);
}

void cBleModuleMeasProgram::start()
{
}

void cBleModuleMeasProgram::stop()
{
}

cBleModuleConfigData *cBleModuleMeasProgram::getConfData()
{
    return qobject_cast<cBleModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cBleModuleMeasProgram::generateVeinInterface()
{
    QString key;
    m_pTemperatureCAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                               key = QString("ACT_TemperatureC"),
                                               QString("Current temperature in degree Celsius"),
                                               QVariant(qQNaN()));
    m_pTemperatureCAct->setUnit("°C");
    m_pTemperatureCAct->setScpiInfo("STATUS", "TEMP:CELSIUS", SCPI::isQuery, m_pTemperatureCAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pTemperatureCAct; // and for the modules interface

    m_pTemperatureFAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                               key = QString("ACT_TemperatureF"),
                                               QString("Current temperature in degree Fahrenheit"),
                                               QVariant(qQNaN()));
    m_pTemperatureFAct->setUnit("°F");
    m_pTemperatureFAct->setScpiInfo("STATUS", "TEMP:FAHRENHEIT", SCPI::isQuery, m_pTemperatureFAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pTemperatureFAct;

    m_pHumidityAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                           key = QString("ACT_Humidity"),
                                           QString("Current relative humidity in percent"),
                                           QVariant(qQNaN()));
    m_pHumidityAct->setUnit("%");
    m_pHumidityAct->setScpiInfo("STATUS", "HUMID", SCPI::isQuery, m_pHumidityAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pHumidityAct;

    m_pAirPressureAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                              key = QString("ACT_AirPressure"),
                                              QString("Current atmospheric pressure in hPa"),
                                              QVariant(qQNaN()));
    m_pAirPressureAct->setUnit("hPa");
    m_pAirPressureAct->setScpiInfo("STATUS", "AIRPR", SCPI::isQuery, m_pAirPressureAct->getName());
    m_pModule->m_veinModuleParameterMap[key] = m_pAirPressureAct;

    m_pWarningFlagsAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                               key = QString("ACT_WarningFlags"),
                                               QString("Current warning flags"),
                                               QVariant((quint32)0));
    m_pModule->m_veinModuleParameterMap[key] = m_pWarningFlagsAct;

    m_pErrorFlagsAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                             key = QString("ACT_ErrorFlags"),
                                             QString("Current error flags"),
                                             QVariant(quint32(0)));
    m_pModule->m_veinModuleParameterMap[key] = m_pErrorFlagsAct;

    m_pBluetoothOnOff = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                              key = QString("PAR_BluetoothOn"),
                                              QString("Bluetooth on"),
                                              QVariant(getConfData()->m_bluetoothOn.m_nActive)); // bool validator ruins true/false
    m_pBluetoothOnOff->setValidator(new cBoolValidator);
    m_pModule->m_veinModuleParameterMap[key] = m_pBluetoothOnOff;
    connect(m_pBluetoothOnOff, &VfModuleComponent::sigValueChanged,
            this, &cBleModuleMeasProgram::onVeinBluetoothOnChanged);

    m_pMacAddress = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->getValidatorEventSystem(),
                                          key = QString("PAR_MacAddress"),
                                          QString("MAC address of environment sensor"),
                                          QVariant("00:00:00:00:00:00"));
    m_pMacAddress->setValidator(new cRegExValidator("^((([0-9A-Fa-f]{2}:){5})|(([0-9A-Fa-f]{2}){5})([0-9A-Fa-f]{2})$|^$)"));

    m_pModule->m_veinModuleParameterMap[key] = m_pMacAddress;
    connect(m_pMacAddress, &VfModuleComponent::sigValueChanged,
            this, &cBleModuleMeasProgram::onVeinMacAddressChanged);
}

void cBleModuleMeasProgram::activateDone()
{
    connect(m_bluetooth, &BluetoothConvenienceFacade::sigOnOff,
            this, &cBleModuleMeasProgram::onBluetoothStatusChanged);
    switchBluetooth(m_pBluetoothOnOff->getValue().toBool());
    m_pMacAddress->setValue(getConfData()->m_macAddress.m_sPar);
    onVeinMacAddressChanged(m_pMacAddress->getValue());
    m_bActive = true;
    emit activated();
}

void cBleModuleMeasProgram::deactivateMeasDone()
{
    emit deactivated();
}

void cBleModuleMeasProgram::onChangeConnectState()
{

}

void cBleModuleMeasProgram::onNewValues()
{
    BluetoothDeviceInfoDecoderPtr decoder = m_bluetooth->findBleDecoder(m_bleDispatcherId);
    if(decoder) {
        EfentoEnvironmentSensor* sensor = static_cast<EfentoEnvironmentSensor*>(decoder.get());
        m_pTemperatureCAct->setValue(sensor->getTemperaturInC());
        m_pTemperatureFAct->setValue(sensor->getTemperaturInF());
        m_pHumidityAct->setValue(sensor->getHumidity());
        m_pAirPressureAct->setValue(sensor->getAirPressure());
        m_pWarningFlagsAct->setValue(sensor->getWarningFlags());
        m_pErrorFlagsAct->setValue(sensor->getErrorFlags());
    }
    else
        makeValuesInvalid();
}

void cBleModuleMeasProgram::makeValuesInvalid()
{
    m_pTemperatureCAct->setValue(qQNaN());
    m_pTemperatureFAct->setValue(qQNaN());
    m_pHumidityAct->setValue(qQNaN());
    m_pAirPressureAct->setValue(qQNaN());
    m_pWarningFlagsAct->setValue(quint32(0));
    m_pErrorFlagsAct->setValue(quint32(0));
}

void cBleModuleMeasProgram::onNewWarnings()
{

}

void cBleModuleMeasProgram::onNewErrors()
{

}

void cBleModuleMeasProgram::onBluetoothStatusChanged(bool on)
{
    if(!on)
        makeValuesInvalid();
    m_pBluetoothOnOff->setValue(int(on));
}

void BLEMODULE::cBleModuleMeasProgram::switchBluetooth(bool on)
{
    if(on)
        m_bluetooth->start();
    else
        m_bluetooth->stop();
}

void cBleModuleMeasProgram::handleDemoActualValues()
{
    m_pTemperatureCAct->setValue(22.1);
    m_pTemperatureFAct->setValue(71.78);
    m_pHumidityAct->setValue(34.0);
    m_pAirPressureAct->setValue(990.9);
    m_pWarningFlagsAct->setValue(quint32(0));
    m_pErrorFlagsAct->setValue(quint32(0));
}

void cBleModuleMeasProgram::onVeinBluetoothOnChanged(QVariant on)
{
    switchBluetooth(on.toBool());
    getConfData()->m_bluetoothOn.m_nActive = on.toInt();
    emit m_pModule->parameterChanged();

    if(m_pModule->getDemo()) {
        if(on.toBool() && m_macAddressForDemo.contains(m_pMacAddress->getValue().toString()))
            handleDemoActualValues();
        else
            makeValuesInvalid();
    }
}

void cBleModuleMeasProgram::onVeinMacAddressChanged(QVariant macAddress)
{
    makeValuesInvalid();
    m_bluetooth->removeBleDecoder(m_bleDispatcherId);
    const QString newMac = macAddress.toString();
    if(!newMac.isEmpty()) {
        std::shared_ptr<EfentoEnvironmentSensor> sensor = std::make_shared<EfentoEnvironmentSensor>();
        sensor->setBluetoothAddress(QBluetoothAddress(newMac));
        connect(sensor.get(), &EfentoEnvironmentSensor::sigChangeConnectState,
                this, &cBleModuleMeasProgram::onChangeConnectState);
        connect(sensor.get(), &EfentoEnvironmentSensor::sigNewValues,
                this, &cBleModuleMeasProgram::onNewValues);
        connect(sensor.get(), &EfentoEnvironmentSensor::sigNewWarnings,
                this, &cBleModuleMeasProgram::onNewWarnings);
        connect(sensor.get(), &EfentoEnvironmentSensor::sigNewErrors,
                this, &cBleModuleMeasProgram::onNewErrors);
        m_bleDispatcherId = m_bluetooth->addBleDecoder(sensor);
        const QString oldMac = getConfData()->m_macAddress.m_sPar;
        if(oldMac != newMac) {
            getConfData()->m_macAddress.m_sPar = newMac;
            emit m_pModule->parameterChanged();
        }
    }
    if(m_pModule->getDemo() && m_pBluetoothOnOff->getValue() != 0) {
        if(m_macAddressForDemo.contains(newMac))
            handleDemoActualValues();
    }
}

}
