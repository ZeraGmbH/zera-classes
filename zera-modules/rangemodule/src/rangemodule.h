#ifndef RANGEMODULE_H
#define RANGEMODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QList>

#include "basemodule.h"

class cRangeMeasChannel;
class cModuleParameter;
class cModuleInfo;
class cRangeModuleConfiguration;
class cRangeModuleMeasProgram;
class cJustifyNormManagement;
class cRangeObsermatic;


namespace Zera {
namespace Server {
 class cDSPInterface;
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

public slots:
    virtual void configXMLInfo(QString key);

protected:
    cRangeModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    cJustifyNormManagement * m_pJustifyNorm; // our justifying and normation program
    cRangeObsermatic *m_pRangeObsermatic; // our range handling
    QList<cRangeMeasChannel*> m_rangeMeasChannelList; // our meas channels
    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void doInitialization(); // here we build our dsp program and start it

private:
    cRangeModuleConfiguration *m_pConfiguration; // our xml configuration
    Zera::Server::cDSPInterface* m_pDSPInterface;
    qint32 m_nActivationIt;
    QStateMachine m_ActivationMachine;
    QState m_ActivationStartState;
    QState m_ActivationExecState;
    QState m_ActivationDoneState;

signals:
    void activationContinue();
    void activationNext();

private slots:
    void activationStart();
    void activationExec();
    void activationDone();

};


#endif // RANGEMODULE_H
