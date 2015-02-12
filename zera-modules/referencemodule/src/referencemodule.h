#ifndef REFERENCEMODULE_H
#define REFERENCEMODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

#include "basemodule.h"




namespace Zera {
namespace Server {
 class cDSPInterface;
}

namespace Proxy {
 class cProxyClient;
}
}

class cModuleInfo;
class cModuleError;

namespace REFERENCEMODULE
{

class cReferenceMeasChannel;
class cReferencecModuleConfiguration;
class cReferenceModuleMeasProgram;
class cReferenceAdjustment;
class cReferenceModuleObservation;

class cReferenceModule : public cBaseModule
{
Q_OBJECT

public:
    cReferenceModule(quint8 modnr, Zera::Proxy::cProxy* proxi, VeinPeer* peer, QObject* parent = 0);
    virtual ~cReferenceModule();
    virtual QByteArray getConfiguration();
    virtual cReferenceMeasChannel* getMeasChannel(QString name); // also used for callback

signals:
    void activationContinue();
    void activationNext();
    void deactivationContinue();
    void deactivationNext();

protected:
    cReferenceModuleObservation *m_pReferenceModuleObservation;
    cReferenceModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    cReferenceAdjustment * m_pReferenceAdjustment; // our justifying and normation program
    QList<cReferenceMeasChannel*> m_ReferenceMeasChannelList; // our meas channels
    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void unsetModule(); // in case of reconfiguration we must unset module first
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
    cModuleError *errorMessage;
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
