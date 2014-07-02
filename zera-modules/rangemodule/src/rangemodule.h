#ifndef RANGEMODULE_H
#define RANGEMODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

#include "basemodule.h"

class cRangeMeasChannel;
class cModuleParameter;
class cModuleInfo;
class cRangeModuleConfiguration;
class cRangeModuleMeasProgram;
class cAdjustManagement;
class cRangeObsermatic;
class cRangeModuleObservation;


namespace Zera {
namespace Server {
 class cDSPInterface;
}

namespace Proxy {
 class cProxyClient;
}
}

class cRangeModule : public cBaseModule
{
Q_OBJECT

public:
    cRangeModule(Zera::Proxy::cProxy* proxi, VeinPeer* peer, QObject* parent = 0);
    virtual ~cRangeModule();
    virtual QByteArray getConfiguration();
    virtual cRangeMeasChannel* getMeasChannel(QString name); // also used for callback

signals:
    void activationContinue();
    void activationNext();
    void deactivationContinue();
    void deactivationNext();

protected:
    cRangeModuleObservation *m_pRangeModuleObservation;
    cRangeModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    cAdjustManagement * m_pAdjustment; // our justifying and normation program
    cRangeObsermatic *m_pRangeObsermatic; // our range handling
    QList<cRangeMeasChannel*> m_rangeMeasChannelList; // our meas channels
    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void unsetModule(); // in case of reconfiguration we must unset module first
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
    cModuleInfo *channelNrInfo;
    cModuleInfo *groupNrInfo;

    //cRangeModuleConfiguration *m_pConfiguration; // our xml configuration
    Zera::Server::cDSPInterface* m_pDSPInterface;
    Zera::Proxy::cProxyClient* m_pDspProxyClient;
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

    void rangeModuleReconfigure();

};


#endif // RANGEMODULE_H
