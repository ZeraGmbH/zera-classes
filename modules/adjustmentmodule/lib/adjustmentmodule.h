#ifndef ADJUSTMENTMODULE_H
#define ADJUSTMENTMODULE_H

#include <basemeasmodule.h>

class cAdjustmentModuleMeasProgram;

class cAdjustmentModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "AdjustmentModule";
    static constexpr const char* BaseSCPIModuleName = "ADJ";
    cAdjustmentModule(MeasurementModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;
protected:
    cAdjustmentModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    virtual void setupModule() override; // after xml configuration we can setup and export our module
    virtual void startMeas() override; // we make the measuring program start here
    virtual void stopMeas() override;

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
private slots:
    void activationStart() override;
    void activationExec() override;
    void activationDone() override;
    void activationFinished() override;

    void deactivationStart() override;
    void deactivationExec() override;
    void deactivationDone() override;
    void deactivationFinished() override;
private:
    qint32 m_nActivationIt;
};

#endif // ADJUSTMENTMODULE_H
