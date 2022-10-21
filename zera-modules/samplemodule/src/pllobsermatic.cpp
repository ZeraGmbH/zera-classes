#include "pllobsermatic.h"
#include "pllmeaschannel.h"
#include "samplemodule.h"
#include "samplechannel.h"
#include <errormessages.h>
#include <scpiinfo.h>
#include <stringvalidator.h>
#include <boolvalidator.h>
#include <math.h>

namespace SAMPLEMODULE
{

cPllObsermatic::cPllObsermatic(cSampleModule* module, Zera::Proxy::cProxy* proxy, cSampleModuleConfigData& confData)
    :m_pModule(module), m_pProxy(proxy), m_ConfPar(confData)
{
    m_getPllMeasChannelsState.addTransition(this, &cPllObsermatic::activationContinue, &m_activationDoneState);
    m_activationMachine.addState(&m_getPllMeasChannelsState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_getPllMeasChannelsState);
    connect(&m_getPllMeasChannelsState, &QState::entered, this, &cPllObsermatic::getPllMeasChannels);
    connect(&m_activationDoneState, &QState::entered, this, &cPllObsermatic::activationDone);

    m_deactivationInitState.addTransition(this, &cPllObsermatic::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, &QState::entered, this, &cPllObsermatic::deactivationInit);
    connect(&m_deactivationDoneState, &QState::entered, this, &cPllObsermatic::deactivationDone);
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
    QString key;
    cSCPIInfo *scpiInfo;

    m_pPllChannel = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             key = QString("PAR_PllChannel"),
                                             QString("PLL reference channel"),
                                             QVariant(m_ConfPar.m_ObsermaticConfPar.m_pllChannel.m_sPar));

    m_pModule->veinModuleParameterHash[key] = m_pPllChannel; // for modules use

    // later we have to set the validator for m_pPLLChannel
    scpiInfo = new cSCPIInfo("CONFIGURATION", "PLLREFERENCE", "10", m_pPllChannel->getName(), "0", "");
    m_pPllChannel->setSCPIInfo(scpiInfo);

    m_pParPllAutomaticOnOff = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_PllAutomaticOnOff"),
                                                       QString("PLL automatic on/off"),
                                                       QVariant(m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive));

    m_pModule->veinModuleParameterHash[key] = m_pParPllAutomaticOnOff; // for modules use

    m_pParPllAutomaticOnOff->setValidator(new cBoolValidator());
    scpiInfo = new cSCPIInfo("CONFIGURATION", "PLLAUTO", "10", m_pParPllAutomaticOnOff->getName(), "0", "");
    m_pParPllAutomaticOnOff->setSCPIInfo(scpiInfo);

    m_pPllSignal = new cVeinModuleComponent(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("SIG_PLL"),
                                            QString("Component forwards information that pll channel is changing"),
                                            QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pPllSignal);
}


void cPllObsermatic::pllAutomatic()
{
    if (m_bActive && (m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive == 1) ) // was automatic configured
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


void cPllObsermatic::setPllChannelValidator()
{
    int i, n;
    QStringList sl;
    cStringValidator *sValidator;

    n = m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.count();

    for (i = 0; i < n; i++)
        sl.append(m_AliasHash[m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(i)]);

    sValidator = new cStringValidator(sl);
    m_pPllChannel->setValidator(sValidator);
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
    m_pParPllAutomaticOnOff->setValue(m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive);

    connect(m_pParPllAutomaticOnOff, &cVeinModuleParameter::sigValueChanged, this, &cPllObsermatic::newPllAuto);
    connect(m_pPllChannel, &cVeinModuleParameter::sigValueChanged, this, &cPllObsermatic::newPllChannel);

    setPllChannelValidator();
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
        m_pPllSignal->setValue(QVariant(1)); // we signal that we are changing pll channel
        m_MsgNrCmdList[m_pllMeasChannelHash[m_sNewPllChannel]->setyourself4PLL(m_ConfPar.m_ObsermaticConfPar.m_sSampleSystem)] = setpll;
    }

    emit m_pModule->parameterChanged();
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
        m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive = 1;
        pllAutomatic(); // call once if switched to automatic
    }
    else
    {
        m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive = 0;
#ifdef DEBUG
        qDebug() << "PLL Automatic off";
#endif
    }

    emit m_pModule->parameterChanged();
}


void cPllObsermatic::catchChannelReply(quint32 msgnr)
{
    int cmd = m_MsgNrCmdList.take(msgnr);
    switch (cmd)
    {
    case setpll:
        m_pPllSignal->setValue(QVariant(0)); // pll change finished
        m_pPllChannel->setValue(QVariant(m_sNewPllChannel));
        m_sActPllChannel = m_sNewPllChannel;
        break;
    }
}

}
