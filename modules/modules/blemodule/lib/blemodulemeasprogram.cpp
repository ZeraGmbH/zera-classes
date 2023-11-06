#include "blemodulemeasprogram.h"
#include "blemodule.h"
#include "blemoduleconfiguration.h"
#include <errormessages.h>
#include <reply.h>

namespace BLEMODULE
{

cBleModuleMeasProgram::cBleModuleMeasProgram(cBleModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration),
    m_pModule(module),
    m_efentoSensor(std::make_shared<EfentoEnvironmentSensor>())
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
    m_pTemperatureCAct = new VfModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              key = QString("ACT_TemperatureC"),
                                              QString("Current temperature in degree celsius"),
                                              QVariant((double)qQNaN()));

    //m_pTemperatureCAct->setSCPIInfo(new cSCPIInfo("CALCULATE", QString("%1:POWER").arg(modNr), "2", m_pTemperatureCAct->getName(), "0", ""));
    m_pModule->veinModuleActvalueList.append(m_pTemperatureCAct); // and for the modules interface
}

void cBleModuleMeasProgram::activateDone()
{
    m_efentoSensor->setBluetoothAddress(QBluetoothAddress("28:2C:02:41:8C:B1")); // TODO from component/settings
    m_bluetoothDispatcher.addBleDecoder(m_efentoSensor);
    connect(m_efentoSensor.get(), &EfentoEnvironmentSensor::sigChangeConnectState,
            this, &cBleModuleMeasProgram::onChangeConnectState);
    connect(m_efentoSensor.get(), &EfentoEnvironmentSensor::sigNewValues,
            this, &cBleModuleMeasProgram::onNewValues);
    connect(m_efentoSensor.get(), &EfentoEnvironmentSensor::sigNewWarnings,
            this, &cBleModuleMeasProgram::onNewWarnings);
    connect(m_efentoSensor.get(), &EfentoEnvironmentSensor::sigNewErrors,
            this, &cBleModuleMeasProgram::onNewErrors);
    m_bluetoothDispatcher.start();
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
    m_pTemperatureCAct->setValue(m_efentoSensor->getTemperaturInC());
}

void cBleModuleMeasProgram::onNewWarnings()
{

}

void cBleModuleMeasProgram::onNewErrors()
{

}

}
