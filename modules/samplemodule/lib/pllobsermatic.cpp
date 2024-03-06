#include "pllobsermatic.h"
#include "pllmeaschannel.h"
#include "samplemodule.h"
#include <errormessages.h>
#include <scpiinfo.h>
#include <stringvalidator.h>
#include <boolvalidator.h>
#include <math.h>

namespace SAMPLEMODULE
{

cPllObsermatic::cPllObsermatic(cSampleModule* module, cSampleModuleConfigData& confData) :
    m_pModule(module),
    m_ConfPar(confData)
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
    pllAutomatic(); // let pllautomatic do its job
}

void cPllObsermatic::generateInterface()
{
    QString key;
    m_pVeinPllChannelAlias = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             key = QString("PAR_PllChannel"),
                                             QString("PLL reference channel"),
                                             QVariant(""));

    m_pModule->m_veinModuleParameterMap[key] = m_pVeinPllChannelAlias; // for modules use

    // later we have to set the validator for m_pPLLChannel
    cSCPIInfo *scpiInfo = new cSCPIInfo("CONFIGURATION", "PLLREFERENCE", "10", m_pVeinPllChannelAlias->getName(), "0", "");
    m_pVeinPllChannelAlias->setSCPIInfo(scpiInfo);

    m_pParPllAutomaticOnOff = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_PllAutomaticOnOff"),
                                                       QString("PLL automatic on/off"),
                                                       QVariant(m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive));

    m_pModule->m_veinModuleParameterMap[key] = m_pParPllAutomaticOnOff; // for modules use

    m_pParPllAutomaticOnOff->setValidator(new cBoolValidator());
    scpiInfo = new cSCPIInfo("CONFIGURATION", "PLLAUTO", "10", m_pParPllAutomaticOnOff->getName(), "0", "");
    m_pParPllAutomaticOnOff->setSCPIInfo(scpiInfo);

    m_pPllFixed = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                       QString("ACT_PllFixed"),
                                       QString("PLL fixed channel / mode"),
                                       QVariant(m_ConfPar.m_ObsermaticConfPar.m_bpllFixed));

    m_pModule->veinModuleComponentList.append(m_pPllFixed);

    m_pPllSignal = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("SIG_PLL"),
                                            QString("Signal on pll channel changing"),
                                            QVariant(0));

    m_pModule->veinModuleComponentList.append(m_pPllSignal);
}

QString cPllObsermatic::getAliasFromSystemName(QString systemName)
{
    if(m_AliasHash.contains(systemName))
        return m_AliasHash[systemName];
    return QString();
}

void cPllObsermatic::pllAutomatic()
{
    if(m_bActive &&
        m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive == 1 &&
        !m_ConfPar.m_ObsermaticConfPar.m_bpllFixed) {
        int i;
        int n = m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.count();
        for (i = 0; i < n; i++) {
            double urValue = m_pllMeasChannelHash[ m_AliasHash[m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(i)]]->getUrValue();
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
    QStringList sl;
    for (int i = 0; i < m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.count(); i++)
        sl.append(m_AliasHash[m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(i)]);

    cStringValidator *sValidator = new cStringValidator(sl);
    m_pVeinPllChannelAlias->setValidator(sValidator);
}

void cPllObsermatic::getPllMeasChannels()
{
    for (int i = 0; i < m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.count(); i++) {
        QString sysName, alias;
        cPllMeasChannel* pllchn;

        sysName = m_ConfPar.m_ObsermaticConfPar.m_pllChannelList.at(i);
        pllchn = m_pModule->getPllMeasChannel(sysName);
        alias = pllchn->getAlias();
        m_pllMeasChannelHash[alias] = pllchn;
        m_AliasHash[sysName] = alias;
    }
    m_pVeinPllChannelAlias->setValue(getAliasFromSystemName(m_ConfPar.m_ObsermaticConfPar.m_pllSystemChannel.m_sPar));
    emit activationContinue();
}

void cPllObsermatic::activationDone()
{
    m_pParPllAutomaticOnOff->setValue(m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive);

    connect(m_pParPllAutomaticOnOff, &VfModuleParameter::sigValueChanged, this, &cPllObsermatic::newPllAuto);
    connect(m_pVeinPllChannelAlias, &VfModuleParameter::sigValueChanged, this, &cPllObsermatic::newPllChannel);

    setPllChannelValidator();
    sendPllChannel(m_ConfPar.m_ObsermaticConfPar.m_pllSystemChannel.m_sPar);

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

void cPllObsermatic::sendPllChannel(QString systemChannelRequested)
{
    if(!m_pllMeasChannelHash.isEmpty()) { // (1 slower)
        if(!m_ConfPar.m_ObsermaticConfPar.m_bpllFixed) {
            QString aliasChannel = getAliasFromSystemName(systemChannelRequested);
            if(!aliasChannel.isEmpty()) {
                m_sNewPllChannelAlias = aliasChannel;
                m_pPllSignal->setValue(QVariant(1)); // we signal that we are changing pll channel
                m_MsgNrCmdList[m_pllMeasChannelHash[aliasChannel]->setyourself4PLL(m_ConfPar.m_ObsermaticConfPar.m_sSampleSystem)] = setpll;
            }
        }
        else {
            m_sNewPllChannelAlias = systemChannelRequested;
            // we abuse 1st channel and get seldom response in catchChannelReply
            // due race on activation state machines of cPllObsermatic (1 slower) & cSampleModule (2 slower)
            m_MsgNrCmdList[m_pllMeasChannelHash.begin().value()->setPLLMode(m_ConfPar.m_ObsermaticConfPar.m_sSampleSystem, systemChannelRequested)] = setpll;
        }
    }
}

void cPllObsermatic::newPllChannel(QVariant channelAlias)
{
    if(!m_ConfPar.m_ObsermaticConfPar.m_bpllFixed) {
        QString systemName = m_pllMeasChannelHash[channelAlias.toString()]->getName();
        // Isn't this obsolete? Vein transfers changes only.
        if(systemName != m_ConfPar.m_ObsermaticConfPar.m_pllSystemChannel.m_sPar)
            sendPllChannel(systemName);
    }
}

// called when pll automatic becomes on or off
void cPllObsermatic::newPllAuto(QVariant pllauto)
{
    if(!m_ConfPar.m_ObsermaticConfPar.m_bpllFixed) {
        if ( pllauto.toInt() == 1) {
            m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive = 1;
            pllAutomatic(); // call once if switched to automatic
        }
        else
            m_ConfPar.m_ObsermaticConfPar.m_npllAutoAct.m_nActive = 0;
        emit m_pModule->parameterChanged();
    }
}

void cPllObsermatic::setNewPLLChannel()
{
    m_pPllSignal->setValue(QVariant(0)); // pll change finished
    m_pVeinPllChannelAlias->setValue(QVariant(m_sNewPllChannelAlias));

    if(m_pllMeasChannelHash.contains(m_sNewPllChannelAlias)) { // (2 slower)
        QString systemName = m_pllMeasChannelHash[m_sNewPllChannelAlias]->getName();
        m_ConfPar.m_ObsermaticConfPar.m_pllSystemChannel.m_sPar = systemName;
        emit m_pModule->parameterChanged();
    }
}

void cPllObsermatic::catchChannelReply(quint32 msgnr)
{
    int cmd = m_MsgNrCmdList.take(msgnr);
    switch (cmd)
    {
    case setpll:
        setNewPLLChannel();
        break;
    }
}

}
