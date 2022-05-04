#ifndef REFERENCEMODULE_H
#define REFERENCEMODULE_H

#include "referencemeaschannel.h"
#include "referencemoduleobservation.h"
#include "referencemodulemeasprogram.h"
#include "referenceadjustment.h"
#include <basemeasmodule.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

namespace REFERENCEMODULE
{
#define BaseModuleName "REFERENCEModule"
#define BaseSCPIModuleName "REF"

class cReferenceModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    cReferenceModule(quint8 modnr, Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    virtual QByteArray getConfiguration() const;
    virtual cReferenceMeasChannel* getMeasChannel(QString name); // also used for callback
protected:
    cReferenceModuleObservation *m_pReferenceModuleObservation;
    cReferenceModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    cReferenceAdjustment *m_pReferenceAdjustment; // our justifying and normation program
    QList<cReferenceMeasChannel*> m_ReferenceMeasChannelList; // our meas channels
    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();

    // our states for base modules activation statemacine
    QState m_ActivationStartState;
    QState m_ActivationExecState;
    QState m_ActivationDoneState;
    QState m_ActivationAdjustmentState;
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
    void activationAdjustment();
    void activationFinished();

    void deactivationStart();
    void deactivationExec();
    void deactivationDone();
    void deactivationFinished();

    void referenceModuleReconfigure();
};

}

#endif // REFERENCEMODULE_H
