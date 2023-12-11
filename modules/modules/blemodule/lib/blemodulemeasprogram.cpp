#include "blemodulemeasprogram.h"
#include "blemodule.h"
#include "blemoduleconfiguration.h"
#include <boolvalidator.h>
#include <regexvalidator.h>
#include <errormessages.h>
#include <reply.h>

namespace BLEMODULE
{

cBleModuleMeasProgram::cBleModuleMeasProgram(cBleModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration),
    m_pModule(module)
{
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_activationDoneState);
    connect(&m_activationDoneState, &QState::entered, this, &cBleModuleMeasProgram::activateDone);

    m_deactivationMachine.addState(&m_deactivateDoneState);
    m_deactivationMachine.setInitialState(&m_deactivateDoneState);
    connect(&m_deactivateDoneState, &QState::entered, this, &cBleModuleMeasProgram::deactivateMeasDone);
}

cBleModuleMeasProgram::~cBleModuleMeasProgram()
{
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

void cBleModuleMeasProgram::generateInterface()
{
    QString key;
    m_pTemperatureCAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                              key = QString("ACT_TemperatureC"),
                                              QString("Current temperature in degree Celsius"),
                                              QVariant((double)qQNaN()));
    m_pTemperatureCAct->setUnit("°C");
    m_pTemperatureCAct->setSCPIInfo(new cSCPIInfo("STATUS", "TEMP:CELSIUS", "2", m_pTemperatureCAct->getName(), "0", m_pTemperatureCAct->getUnit()));

    m_pModule->veinModuleParameterHash[key] = m_pTemperatureCAct; // and for the modules interface

    m_pTemperatureFAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("ACT_TemperatureF"),
                                             QString("Current temperature in degree Fahrenheit"),
                                             QVariant((double)qQNaN()));
    m_pTemperatureFAct->setUnit("°F");
    m_pTemperatureFAct->setSCPIInfo(new cSCPIInfo("STATUS", "TEMP:FAHRENHEIT", "2", m_pTemperatureFAct->getName(), "0", m_pTemperatureFAct->getUnit()));
    m_pModule->veinModuleParameterHash[key] = m_pTemperatureFAct;

    m_pHumidityAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                          key = QString("ACT_Humidity"),
                                          QString("Current relative humidity in percent"),
                                          QVariant((double)qQNaN()));
    m_pHumidityAct->setUnit("%");
    m_pHumidityAct->setSCPIInfo(new cSCPIInfo("STATUS", "HUMID", "2", m_pHumidityAct->getName(), "0", m_pHumidityAct->getUnit()));
    m_pModule->veinModuleParameterHash[key] = m_pHumidityAct;

    m_pAirPressureAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("ACT_AirPressure"),
                                             QString("Current atmospheric pressure in hPa"),
                                             QVariant((double)qQNaN()));
    m_pAirPressureAct->setUnit("hPa");
    m_pAirPressureAct->setSCPIInfo(new cSCPIInfo("STATUS", "AIRPR", "2", m_pAirPressureAct->getName(), "0", m_pAirPressureAct->getUnit()));
    m_pModule->veinModuleParameterHash[key] = m_pAirPressureAct;

    m_pWarningFlagsAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                              key = QString("ACT_WarningFlags"),
                                              QString("Current warning flags"),
                                              QVariant((quint16)qQNaN()));

    m_pErrorFlagsAct = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            key = QString("ACT_ErrorFlags"),
                                            QString("Current error flags"),
                                            QVariant((quint32)qQNaN()));

    m_pBluetoothOnOff = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                key = QString("PAR_BluetoothOn"),
                                QString("Bluetooth on"),
                                QVariant(getConfData()->m_bluetoothOn.m_nActive)); // bool validator ruins true/false
    m_pBluetoothOnOff->setValidator(new cBoolValidator);
    m_pModule->veinModuleParameterHash[key] = m_pBluetoothOnOff;
    connect(m_pBluetoothOnOff, &VfModuleComponent::sigValueChanged,
            this, &cBleModuleMeasProgram::onVeinBluetoothOnChanged);

    m_pMacAddress = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                          key = QString("PAR_MacAddress"),
                                          QString("MAC address of environment sensor"),
                                          QVariant("00:00:00:00:00:00"));
    m_pMacAddress->setValidator(new cRegExValidator("^((([0-9A-Fa-f]{2}:){5})|(([0-9A-Fa-f]{2}){5})([0-9A-Fa-f]{2})$|^$)"));

    m_pModule->veinModuleParameterHash[key] = m_pMacAddress;
    connect(m_pMacAddress, &VfModuleComponent::sigValueChanged,
            this, &cBleModuleMeasProgram::onVeinMacAddressChanged);
}

void cBleModuleMeasProgram::activateDone()
{
    connect(&m_bluetooth, &BluetoothConvenienceFacade::sigOnOff,
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
    BluetoothDeviceInfoDecoderPtr decoder = m_bluetooth.findBleDecoder(m_bleDispatcherId);
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
    m_pWarningFlagsAct->setValue(0);
    m_pErrorFlagsAct->setValue(0);
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
        m_bluetooth.start();
    else
        m_bluetooth.stop();
}

void cBleModuleMeasProgram::handleDemoActualValues()
{
    m_pTemperatureCAct->setValue("22.1");
    m_pTemperatureFAct->setValue("71.78");
    m_pHumidityAct->setValue("34.0");
    m_pAirPressureAct->setValue("990.9");
    m_pWarningFlagsAct->setValue(0);
    m_pErrorFlagsAct->setValue(0);
}

void cBleModuleMeasProgram::onVeinBluetoothOnChanged(QVariant on)
{
    switchBluetooth(on.toBool());
    getConfData()->m_bluetoothOn.m_nActive = on.toInt();
    emit m_pModule->parameterChanged();

    if(m_pModule->m_demo) {
        if(on.toBool() && m_macAddressForDemo.contains(m_pMacAddress->getValue().toString()))
            handleDemoActualValues();
        else
            makeValuesInvalid();
    }
}

void cBleModuleMeasProgram::onVeinMacAddressChanged(QVariant macAddress)
{
    makeValuesInvalid();
    m_bluetooth.removeBleDecoder(m_bleDispatcherId);
    if(!macAddress.toString().isEmpty()) {
        std::shared_ptr<EfentoEnvironmentSensor> sensor = std::make_shared<EfentoEnvironmentSensor>();
        sensor->setBluetoothAddress(QBluetoothAddress(macAddress.toString()));
        connect(sensor.get(), &EfentoEnvironmentSensor::sigChangeConnectState,
                this, &cBleModuleMeasProgram::onChangeConnectState);
        connect(sensor.get(), &EfentoEnvironmentSensor::sigNewValues,
                this, &cBleModuleMeasProgram::onNewValues);
        connect(sensor.get(), &EfentoEnvironmentSensor::sigNewWarnings,
                this, &cBleModuleMeasProgram::onNewWarnings);
        connect(sensor.get(), &EfentoEnvironmentSensor::sigNewErrors,
                this, &cBleModuleMeasProgram::onNewErrors);
        m_bleDispatcherId = m_bluetooth.addBleDecoder(sensor);
        getConfData()->m_macAddress.m_sPar = macAddress.toString();
        emit m_pModule->parameterChanged();
    }
    if(m_pModule->m_demo && m_pBluetoothOnOff->getValue() != 0) {
        if(m_macAddressForDemo.contains(macAddress.toString()))
            handleDemoActualValues();
    }
}

}
