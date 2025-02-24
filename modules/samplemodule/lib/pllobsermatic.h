#ifndef PLLOBSERMATIC_H
#define PLLOBSERMATIC_H

// cPllObsermatic is responsible for setting, observing, automatic setting
// of pll channels. it works on a flexible list of system measurement channels.
// it provides its interface :
// ChannelXRange Input/Output for range setting
// ChannelXOVR   Output information of overload conditions
// !!! important pll obsermatic must be activated after !!! activating pllchannels

#include "samplemoduleconfigdata.h"
#include "pllmeaschannel.h"
#include <moduleactivist.h>
#include <vfmoduleparameter.h>

namespace SAMPLEMODULE
{
enum sampleObsermaticCmds
{
    setpll
};

class cSampleModule;

class cPllObsermatic: public cModuleActivist
{
    Q_OBJECT
public:
    cPllObsermatic(cSampleModule* module, cSampleModuleConfigData& confData);
    virtual ~cPllObsermatic();
    void generateVeinInterface() override;
    QString getAliasFromSystemName(QString systemName);
public slots:
    virtual void ActionHandler(QVector<float>* actualValues); // entry after received actual values
    void catchChannelReply(quint32 msgnr);
private slots:
    void getPllMeasChannels();
    void activationDone();

    void deactivationInit();
    void deactivationDone();

    void newPllChannel(QVariant channelAlias);
    void newPllAuto(QVariant pllauto);
private:
    void pllAutomatic();
    void setPllChannelValidator();
    void sendPllChannel(QString systemChannelRequested);
    void setNewPLLChannel();

    cSampleModule *m_pModule;
    cSampleModuleConfigData& m_ConfPar;
    QString m_sNewPllChannelAlias;
    QVector<float> m_ActualValues; // here we find the actual values

    // we build up 2 hashtables for pll meas channel access
    QHash<QString,cPllMeasChannel*> m_pllMeasChannelHash; // alias -> pllmeaschannel
    QHash<QString,QString> m_AliasHash; // systemname -> alias

    // we need some entities for our interface
    VfModuleParameter* m_pVeinPllChannelAlias;
    VfModuleParameter* m_pParPllAutomaticOnOff;
    VfModuleComponent* m_pPllFixed;
    VfModuleComponent* m_pPllSignal; // we make the signal public for easy connection within module

    QHash<quint32, int> m_MsgNrCmdList;

    // statemachine for activation
    QState m_getPllMeasChannelsState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;
};

}

#endif // PLLOBSERMATIC_H
