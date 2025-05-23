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

    VfModuleParameter* m_pTemperatureCAct;
    VfModuleParameter* m_pTemperatureFAct;
    VfModuleParameter* m_pHumidityAct;
    VfModuleParameter* m_pAirPressureAct;
    VfModuleParameter* m_pErrorFlagsAct;
    VfModuleParameter* m_pWarningFlagsAct;
    VfModuleParameter* m_pBluetoothOnOff;
    VfModuleParameter* m_pMacAddress;

    QStringList m_macAddressForDemo = {"11:22:33:44:55:66", "AA:BB:CC:DD:EE:FF", "00:00:00:00:00:01", "00:00:00:00:00:00"};
};

}
#endif // BLEMODULEMEASPROGRAM_H
