#ifndef OSCIMODULE_H
#define OSCIMODULE_H

#include "basemeasmodule.h"
#include <QFinalState>

namespace OSCIMODULE {

class cOsciModuleConfiguration;
class cOsciModuleMeasProgram;

class cOsciModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "OSCIModule";
    static constexpr const char* BaseSCPIModuleName = "OSC";

    cOsciModule(MeasurementModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;

protected:
    cOsciModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();

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

#endif // OSCIMODULE_H
