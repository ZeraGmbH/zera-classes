#ifndef SEC1MODULE_H
#define SEC1MODULE_H

#include <basemeasmodule.h>

namespace SEC1MODULE
{
class cSec1ModuleMeasProgram;

class cSec1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SEC1Module";
    // shortcut of scpi module name is only first 4 characters
    // so we will provide ecalculator type EC01 .. EC99
    // and each ec will have an additional scpi parent with its number 0001 .. 9999
    static constexpr const char* BaseSCPIModuleName = "EC01";

    cSec1Module(MeasurementModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;
protected:
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();

    cSec1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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
    void activationStart();
    void activationExec();
    void activationDone();
    void activationFinished();

    void deactivationStart();
    void deactivationExec();
    void deactivationDone();
    void deactivationFinished();

};

}

#endif // SEC1MODULE_H
