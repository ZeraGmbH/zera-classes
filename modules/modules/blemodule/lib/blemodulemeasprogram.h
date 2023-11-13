#ifndef BLEMODULEMEASPROGRAM_H
#define BLEMODULEMEASPROGRAM_H

#include "blemoduleconfigdata.h"
#include "basemeasworkprogram.h"
#include "vfmoduleactvalue.h"
#include "vfmoduleparameter.h"
#include <bluetoothconveniencefacade.h>
#include <efentoenvironmentsensor.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace BLEMODULE
{

class cBleModule;
class cBleModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    cBleModuleMeasProgram(cBleModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cBleModuleMeasProgram();
    void generateInterface() override; // here we export our interface (entities)
public slots:
    void start() override; // difference between start and stop is that actual values
    void stop() override; // in interface are not updated when stop

private slots:
    void activateDone();
    void deactivateMeasDone();
    void onChangeConnectState();
    void onNewValues();
    void onNewWarnings();
    void onNewErrors();
    void onBluetoothStatusChanged(bool on);
    void onVeinBluetoothOnChanged(QVariant on);
    void onVeinMacAddressChanged(QVariant macAddress);
private:
    cBleModuleConfigData* getConfData();
    void makeValuesInvalid();

    cBleModule* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on

    // statemachine for activating gets the following states
    QFinalState m_activationDoneState;
    // statemachine for deactivating
    QFinalState m_deactivateDoneState;

    BluetoothConvenienceFacade m_bluetooth;
    BleDispatcherId m_bleDispatcherId;

    VfModuleActvalue* m_pTemperatureCAct;
    VfModuleActvalue* m_pTemperatureFAct;
    VfModuleActvalue* m_pHumidityAct;
    VfModuleActvalue* m_pAirPressureAct;
    VfModuleActvalue* m_pErrorFlagsAct;
    VfModuleActvalue* m_pWarningFlagsAct;
    VfModuleParameter* m_pBluetoothOnOff;
    VfModuleParameter* m_pMacAddress;
};

}
#endif // BLEMODULEMEASPROGRAM_H
