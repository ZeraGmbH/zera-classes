#ifndef BLEMODULE_H
#define BLEMODULE_H

#include "blemodulemeasprogram.h"
#include <basemeasmodule.h>

namespace BLEMODULE
{
class cBleModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "BleModule";
    static constexpr const char* BaseSCPIModuleName = "BLE";

    cBleModule(MeasurementModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;

protected:
    cBleModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    // our states for base modules activation statemacine
    QState m_ActivationStartState;
    QState m_ActivationExecState;
    QState m_ActivationDoneState;
    QFinalState m_ActivationFinishedState;

    // our states for base modules deactivation statemacine
    QState m_DeactivationStartState;
    QState m_DeactivationExecState;
    QState m_DeactivationDoneState;
    QFinalState m_DeactivationFinishedState;

private:
    qint32 m_nActivationIt;

private slots:
    void activationStart() override;
    void activationExec() override;
    void activationDone() override;
    void activationFinished() override;

    void deactivationStart() override;
    void deactivationExec() override;
    void deactivationDone() override;
    void deactivationFinished() override;
};

}

#endif // BLEMODULE_H
