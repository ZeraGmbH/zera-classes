#ifndef PLLOBSERMATIC_H
#define PLLOBSERMATIC_H


// cPllObsermatic is responsible for setting, observing, automatic setting
// of pll channels. it works on a flexible list of system measurement channels.
// it provides its interface :
// ChannelXRange Input/Output for range setting
// ChannelXOVR   Output information of overload conditions
// !!! important pll obsermatic must be activated after !!! activating pllchannels

#include <QObject>
#include <QList>
#include <QVector>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QVariant>
#include <QStringList>

#include "moduleactivist.h"
#include "samplemoduleconfigdata.h"


namespace Zera {
namespace Proxy {
    class cProxy;
    class cProxyClient;
}

namespace  Server {
    class cDSPInterface;
}
}


class cDspMeasData;
class cVeinModuleParameter;
class cVeinModuleComponent;

namespace SAMPLEMODULE
{

enum sampleObsermaticCmds
{
    setpll
};


class cSampleModule;
class cPllMeasChannel;

class cPllObsermatic: public cModuleActivist
{
    Q_OBJECT

public:
    cPllObsermatic(cSampleModule* module, Zera::Proxy::cProxy* proxy, cSampleModuleConfigData& confData);
    virtual ~cPllObsermatic();
    virtual void generateInterface(); // here we export our interface (entities)


public slots:
    virtual void ActionHandler(QVector<float>* actualValues); // entry after received actual values
    void catchChannelReply(quint32 msgnr);

private:
    cSampleModule *m_pModule;
    Zera::Proxy::cProxy* m_pProxy; // the proxy where we can get our connections
    cSampleModuleConfigData& m_ConfPar;
    QString m_sActPllChannel;
    QString m_sNewPllChannel;
    QVector<float> m_ActualValues; // here we find the actual values

    // we build up 2 hashtables for pll meas channel access
    QHash<QString,cPllMeasChannel*> m_pllMeasChannelHash; // alias -> pllmeaschannel
    QHash<QString,QString> m_AliasHash; // systemname -> alias

    // we need some entities for our interface
    cVeinModuleParameter* m_pPllChannel;
    cVeinModuleParameter* m_pParPllAutomaticOnOff;
    cVeinModuleComponent* m_pPllSignal; // we make the signal public for easy connection within module

    QHash<quint32, int> m_MsgNrCmdList;

    // statemachine for activation
    QState m_getPllMeasChannelsState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;

    bool m_bPllAutomatic;
    void pllAutomatic();

    void setPllChannelValidator();

private slots:
    void getPllMeasChannels();
    void activationDone();

    void deactivationInit();
    void deactivationDone();

    void newPllChannel(QVariant channel);
    void newPllAuto(QVariant pllauto);
};

}

#endif // PLLOBSERMATIC_H
