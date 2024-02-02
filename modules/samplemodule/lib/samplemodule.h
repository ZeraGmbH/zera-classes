#ifndef SAMPLEMODULE_H
#define SAMPLEMODULE_H

#include "samplemodulemeasprogram.h"
#include "samplechannel.h"
#include "pllmeaschannel.h"
#include "pllobsermatic.h"
#include <basemeasmodule.h>

namespace SAMPLEMODULE
{
class cSampleModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SampleModule";
    static constexpr const char* BaseSCPIModuleName = "SAM";

    cSampleModule(quint8 modnr, int entityId, VeinEvent::StorageSystem* storagesystem, bool demo);
    QByteArray getConfiguration() const override;
    cPllMeasChannel* getPllMeasChannel(QString name);

protected:
    cSampleModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    cPllObsermatic *m_pPllObsermatic; // our pll handling

    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
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
    QList<cPllMeasChannel*> m_pllMeasChannelList; // our pll meas channels
    QList<cSampleChannel*>  m_sampleChannelList;

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

#endif // SAMPLEMODULE_H
