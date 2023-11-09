#include "blemodulemeasprogram.h"
#include "blemodule.h"
#include "blemoduleconfiguration.h"
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

    connect(&m_bleDiscoverer, &BleDeviceDisoverer::sigDeviceDiscovered,
            &m_bleDispatcher, &BleDeviceInfoDispatcher::onDeviceDiscovered);
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
    m_pTemperatureCAct = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("ACT_TemperatureC"),
                                              QString("Current temperature in degree Celsius"),
                                              QVariant((double)qQNaN()));
    //m_pTemperatureCAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:POWER").arg(modNr), "2", m_pTemperatureCAct->getName(), "0", ""));
    m_pTemperatureCAct->setUnit("°C");
    m_pModule->veinModuleActvalueList.append(m_pTemperatureCAct); // and for the modules interface

    m_pTemperatureFAct = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             key = QString("ACT_TemperatureF"),
                                             QString("Current temperature in degree Fahrenheit"),
                                             QVariant((double)qQNaN()));
    m_pTemperatureFAct->setUnit("°F");
    // SCPI ToDo
    m_pModule->veinModuleActvalueList.append((m_pTemperatureFAct));

    m_pHumidityAct = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                          key = QString("ACT_Humidity"),
                                          QString("Current relative humidity in percent"),
                                          QVariant((double)qQNaN()));
    m_pHumidityAct->setUnit("%");   // todo: make this problems?
    // SCPI ToDo
    m_pModule->veinModuleActvalueList.append(m_pHumidityAct);

    m_pAirPressureAct = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             key = QString("ACT_AirPressure"),
                                             QString("Current atmospheric pressure in hPa"),
                                             QVariant((double)qQNaN()));
    m_pAirPressureAct->setUnit("hPa");
    // SCPI ToDo
    m_pModule->veinModuleActvalueList.append(m_pAirPressureAct);

    m_pWarningFlagsAct = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("ACT_WarningFlags"),
                                              QString("Current warning flags"),
                                              QVariant((quint16)qQNaN()));
    m_pModule->veinModuleActvalueList.append(m_pWarningFlagsAct);

    m_pErrorFlagsAct = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            key = QString("ACT_ErrorFlags"),
                                            QString("Current error flags"),
                                            QVariant((quint32)qQNaN()));

    m_pMacAddress = new VfModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                          key = QString("PAR_MacAddress"),
                                          QString("MAC address of environment sensor"),
                                          QVariant(""));
    m_pMacAddress->setValidator(new cRegExValidator("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$|^$"));
    m_pModule->veinModuleParameterHash[key] = m_pMacAddress;
    connect(m_pMacAddress, &VfModuleComponent::sigValueChanged,
            this, &cBleModuleMeasProgram::onMacAddressChanged);
}

void cBleModuleMeasProgram::activateDone()
{
    m_bleDiscoverer.start();
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
    BluetoothDeviceInfoDecoderPtr decoder = m_bleDispatcher.removeBleDecoder(m_bleDispatcherId);
    if(decoder) {
        EfentoEnvironmentSensor* sensor = static_cast<EfentoEnvironmentSensor*>(decoder.get());
        m_pTemperatureCAct->setValue(sensor->getTemperaturInC());
        m_pTemperatureFAct->setValue(sensor->getTemperaturInF());
        m_pHumidityAct->setValue(sensor->getHumidity());
        m_pAirPressureAct->setValue(sensor->getAirPressure());
        m_pWarningFlagsAct->setValue(sensor->getWarningFlags());
        m_pErrorFlagsAct->setValue(sensor->getErrorFlags());

        m_bleDispatcherId = m_bleDispatcher.addBleDecoder(std::move(decoder));
    }
    else
        makeValueInvalid();
}

void cBleModuleMeasProgram::makeValueInvalid()
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

void cBleModuleMeasProgram::onMacAddressChanged(QVariant macAddress)
{
    makeValueInvalid();
    if(!macAddress.toString().isEmpty()) {
        std::unique_ptr<EfentoEnvironmentSensor> sensor = std::make_unique<EfentoEnvironmentSensor>();
        sensor->setBluetoothAddress(QBluetoothAddress(macAddress.toString()));
        connect(sensor.get(), &EfentoEnvironmentSensor::sigChangeConnectState,
                this, &cBleModuleMeasProgram::onChangeConnectState);
        connect(sensor.get(), &EfentoEnvironmentSensor::sigNewValues,
                this, &cBleModuleMeasProgram::onNewValues);
        connect(sensor.get(), &EfentoEnvironmentSensor::sigNewWarnings,
                this, &cBleModuleMeasProgram::onNewWarnings);
        connect(sensor.get(), &EfentoEnvironmentSensor::sigNewErrors,
                this, &cBleModuleMeasProgram::onNewErrors);
        m_bleDispatcherId = m_bleDispatcher.addBleDecoder(std::move(sensor));
    }
    else
        m_bleDispatcher.removeBleDecoder(m_bleDispatcherId);
}

}
