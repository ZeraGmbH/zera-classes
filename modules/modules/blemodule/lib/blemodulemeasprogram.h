#ifndef BLEMODULEMEASPROGRAM_H
#define BLEMODULEMEASPROGRAM_H

#include "blemoduleconfigdata.h"
#include "basemeasworkprogram.h"
#include "vfmoduleactvalue.h"
#include <bledeviceinfodispatcher.h>
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
private:
    cBleModuleConfigData* getConfData();

    cBleModule* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on

    // statemachine for activating gets the following states
    QFinalState m_activationDoneState;
    // statemachine for deactivating
    QFinalState m_deactivateDoneState;

    BleDeviceInfoDispatcher m_bluetoothDispatcher;
    std::shared_ptr<EfentoEnvironmentSensor> m_efentoSensor;
    VfModuleActvalue* m_pTemperatureCAct;
};

}
#endif // BLEMODULEMEASPROGRAM_H
