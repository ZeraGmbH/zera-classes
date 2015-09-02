#include <QString>
#include <QVector>
#include <QPoint>
#include <math.h>
#include <veinpeer.h>
#include <veinentity.h>
#include <dspinterface.h>
#include <proxy.h>
#include <proxyclient.h>

#include "debug.h"
#include "errormessages.h"
#include "samplemodule.h"
#include "pllmeaschannel.h"
#include "moduleparameter.h"
#include "interfaceentity.h"
#include "moduleinfo.h"
#include "modulesignal.h"
#include "pllobsermatic.h"
#include "samplechannel.h"

namespace SAMPLEMODULE
{

cPllObsermatic::cPllObsermatic(cSampleModule* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, cSampleModuleConfigData& confData)
    :m_pModule(module), m_pProxy(proxy), m_pPeer(peer), m_ConfPar(confData)
{
    m_getPllMeasChannelsState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);
    m_activationMachine.addState(&m_getPllMeasChannelsState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_getPllMeasChannelsState);
    connect(&m_getPllMeasChannelsState, SIGNAL(entered()), SLOT(getPllMeasChannels()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    m_deactivationInitState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, SIGNAL(entered()), SLOT(deactivationInit()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));
}


cPllObsermatic::~cPllObsermatic()
{
}


void cPllObsermatic::ActionHandler(QVector<float> *actualValues)
{
    m_ActualValues = *actualValues;
#ifdef DEBUG
    qDebug() << QString("RMS %1 ; %2 ; %3 ;").arg(m_ActualValues[0]).arg(m_ActualValues[1]).arg(m_ActualValues[2])
             << QString("%1 ; %2 ; %3").arg(m_ActualValues[3]).arg(m_ActualValues[4]).arg(m_ActualValues[5]);
#endif

    pllAutomatic(); // let pllautomatic do its job
}


void cPllObsermatic::generateInterface()
{
    QString s;

    m_pPllChannelEntity = m_pPeer->dataAdd(QString("PAR_PLLChannel"));
    m_pPllChannelEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
    m_pPllChannelEntity->setValue(QVariant(s = "Unknown"));

    m_pPllChannelListEntity = m_pPeer->dataAdd(QString("INF_PLLChannelList")); // list of possible pll channels
    m_pPllChannelListEntity->modifiersAdd(VeinEntity::MOD_READONLY);
    m_pPllChannelListEntity->setValue(s = "Unknown", m_pPeer);

    m_pParPllAutomaticOnOff = new cModuleParameter(m_pPeer, "PAR_PllAutomaticON/OFF", m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive, !m_ConfPar.m_ObsermaticConfPar.m_bpllAuto);
    m_pPllSignal = new cModuleSignal(m_pPeer, "SIG_PLL", QVariant(0));
}


void cPllObsermatic::deleteInterface()
{
    m_pPeer->dataRemove(m_pPllChannelEntity);
    m_pPeer->dataRemove(m_pPllChannelListEntity);

    delete m_pParPllAutomaticOnOff;
    delete m_pPllSignal;
}


void cPllObsermatic::exportInterface(QJsonArray &jsArr)
{
    cInterfaceEntity ifaceEntity;

    ifaceEntity.setName(m_pPllChannelEntity->getName());
    ifaceEntity.setDescription(QString("This entity holds the pll reference channel"));
    ifaceEntity.setSCPIModel(QString("CONFIGURATION"));
    ifaceEntity.setSCPICmdnode(QString("PLLREFERENCE"));
    ifaceEntity.setSCPIType(QString("10"));
    ifaceEntity.setUnit(QString(""));

    ifaceEntity.appendInterfaceEntity(jsArr);

    ifaceEntity.setName(m_pParPllAutomaticOnOff->getName());
    ifaceEntity.setDescription(QString("This entity selects pll automatic"));
    ifaceEntity.setSCPICmdnode(QString("PLLAUTO"));

    ifaceEntity.appendInterfaceEntity(jsArr);

}


void cPllObsermatic::pllAutomatic()
{
    if (m_bActive && m_ConfPar.m_ObsermaticConfPar.m_bpllAuto) // was automatic configured
    {
        int i, n;
        n = m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.count();
        for (i = 0; i < n; i++)
        {
            double urValue;
            urValue = m_pllMeasChannelHash[ m_AliasHash[m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(i)]]->getUrValue();
            if (m_ActualValues[i] > urValue * 10.0 / 100.0)
                break;
        }

        if (i == n)
            i = 0; // if none of our channels has 10% attenuation we take the first channel

        // now we set our new pll channel
        newPllChannel(QVariant(m_AliasHash[m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(i)]));
    }
}


void cPllObsermatic::setPllChannelListEntity()
{
    int i, n;
    QString s;

    n = m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.count();
    s = m_AliasHash[m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(0)];
    if (n > 1)
        for (i = 1; i < n; i++)
            s = s + ";" + m_AliasHash[m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(i)];

    m_pPllChannelListEntity->setValue(s, m_pPeer);
}


void cPllObsermatic::getPllMeasChannels()
{
    int i, n;

    n = m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.count();
    for (i = 0; i < n; i++)
    {
        QString sysName, alias;
        cPllMeasChannel* pllchn;

        sysName = m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(i);
        pllchn = m_pModule->getPllMeasChannel(sysName);
        alias = pllchn->getAlias();
        m_pllMeasChannelHash[alias] = pllchn;
        m_AliasHash[sysName] = alias;
    }

    emit activationContinue();
}


void cPllObsermatic::activationDone()
{
    m_pParPllAutomaticOnOff->setData(m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive);

    connect(m_pParPllAutomaticOnOff, SIGNAL(updated(QVariant)), this, SLOT(newPllAuto(QVariant)));
    connect(m_pPllChannelEntity, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newPllChannel(QVariant)));

    setPllChannelListEntity();
    newPllChannel(QVariant(m_ConfPar.m_ObsermaticConfPar.m_pllChannel.m_sPar)); // we set our default channel here

    m_bActive = true;
    emit activated();
}


void cPllObsermatic::deactivationInit()
{
    m_bActive = false;
    emit deactivationContinue();
}


void cPllObsermatic::deactivationDone()
{
    emit deactivated();
}


void cPllObsermatic::newPllChannel(QVariant channel)
{
    m_sNewPllChannel = channel.toString();
    if (!m_pllMeasChannelHash.contains(m_sNewPllChannel))
        m_sNewPllChannel = m_AliasHash[m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(0)];

    if (m_sNewPllChannel != m_sActPllChannel)
    {
#ifdef DEBUG
        qDebug() << QString("New PLL-Channel: %1 selected").arg(m_sNewPllChannel);
#endif
        m_pPllSignal->m_pParEntity->setValue(QVariant(1), m_pPeer); // we signal that we are changing pll channel
        m_MsgNrCmdList[m_pllMeasChannelHash[m_sNewPllChannel]->setyourself4PLL(m_ConfPar.m_ObsermaticConfPar.m_sSampleSystem)] = setpll;
    }
}


// called when pll automatic becomes on or off
void cPllObsermatic::newPllAuto(QVariant pllauto)
{
    bool ok;

    if ( (m_bPllAutomatic = (pllauto.toInt(&ok) == 1)) )
    {
#ifdef DEBUG
        qDebug() << "PLL Automatic on";
#endif
        m_ConfPar.m_ObsermaticConfPar.m_bpllAuto = true;
        pllAutomatic(); // call once if switched to automatic
    }
    else
    {
        m_ConfPar.m_ObsermaticConfPar.m_bpllAuto = false;
#ifdef DEBUG
        qDebug() << "PLL Automatic off";
#endif
    }
}


void cPllObsermatic::catchChannelReply(quint32 msgnr)
{
    int cmd = m_MsgNrCmdList.take(msgnr);
    switch (cmd)
    {
    case setpll:
        m_pPllSignal->m_pParEntity->setValue(QVariant(0), m_pPeer); // pll change finished
        m_sActPllChannel = m_sNewPllChannel;
        disconnect(m_pPllChannelEntity, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newPllChannel(QVariant)));
        m_pPllChannelEntity->setValue(m_sActPllChannel, m_pPeer); //
        connect(m_pPllChannelEntity, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newPllChannel(QVariant)));
        break;
    }
}

}
