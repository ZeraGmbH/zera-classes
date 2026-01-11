#include "pllobsermatic.h"
#include "samplemodule.h"
#include <errormessages.h>
#include <scpi.h>
#include <stringvalidator.h>
#include <boolvalidator.h>

namespace SAMPLEMODULE
{

/*cPllObsermatic::cPllObsermatic(cSampleModule* module, cSampleModuleConfigData& confData) :
    cModuleActivist(module->getVeinModuleName()),
    m_pModule(module),
    m_ConfPar(confData)
{
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_activationDoneState);
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

void cPllObsermatic::generateVeinInterface()
{
}

QString cPllObsermatic::getAliasFromSystemName(QString systemName)
{
    if(m_AliasHash.contains(systemName))
        return m_AliasHash[systemName];
    return QString();
}

void cPllObsermatic::activationDone()
{

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


void cPllObsermatic::setNewPLLChannel()
{
    m_pPllSignal->setValue(QVariant(0)); // pll change finished
    m_pVeinPllChannelAlias->setValue(QVariant(m_sNewPllChannelAlias));

    if(m_pllMeasChannelHash.contains(m_sNewPllChannelAlias)) { // (2 slower)
        QString systemName = m_pllMeasChannelHash[m_sNewPllChannelAlias]->getMName();
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
}*/

}
