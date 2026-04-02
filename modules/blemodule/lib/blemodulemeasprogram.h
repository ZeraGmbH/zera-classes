#ifndef BLEMODULEMEASPROGRAM_H
#define BLEMODULEMEASPROGRAM_H

#include "blemoduleconfigdata.h"
#include "basemeasworkprogram.h"
#include "vfmoduleparameter.h"
#include <bluetoothconveniencefacade.h>
#include <efentoenvironmentsensor.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QStringList>

namespace BLEMODULE
{

class cBleModule;
class cBleModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    cBleModuleMeasProgram(cBleModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    virtual ~cBleModuleMeasProgram();
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;

private slots:
    void activateDone();
    void deactivateMeasDone();
    void onNewValues();
    void onBluetoothStatusChanged(bool on);
    void onVeinBluetoothOnChanged(QVariant on);
    void onVeinMacAddressChanged(QVariant macAddress);
private:
    cBleModuleConfigData* getConfData();
    void makeValuesInvalid();
    void switchBluetooth(bool on);
    void handleDemoActualValues();

    cBleModule* m_pModule;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on

    // statemachine for activating gets the following states
    QFinalState m_activationDoneState;
    // statemachine for deactivating
    QFinalState m_deactivateDoneState;

    BluetoothConvenienceFacade *m_bluetooth;
    BleDispatcherId m_bleDispatcherId;

    VfModuleParameter* m_deviceSerialNo = nullptr;
    VfModuleParameter* m_pTemperatureCAct = nullptr;
    VfModuleParameter* m_pTemperatureFAct = nullptr;
    VfModuleParameter* m_pHumidityAct = nullptr;
    VfModuleParameter* m_pAirPressureAct = nullptr;
    VfModuleParameter* m_pErrorFlagsAct = nullptr;
    VfModuleParameter* m_pWarningFlagsAct = nullptr;
    VfModuleParameter* m_pBluetoothOnOff = nullptr;
    VfModuleParameter* m_pMacAddress = nullptr;

    QStringList m_macAddressForDemo = {"112233445566", "AABBCCDDEEFF", "000000000001", "000000000000"};
};

}
#endif // BLEMODULEMEASPROGRAM_H
