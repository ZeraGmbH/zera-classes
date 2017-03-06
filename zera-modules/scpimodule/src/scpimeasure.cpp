#include <QVariant>

#include <scpi.h>

#include "scpimodule.h"
#include "scpicmdinfo.h"
#include "scpimeasure.h"
#include "moduleinterface.h"


namespace SCPIMODULE
{

cSCPIMeasure::cSCPIMeasure(cSCPIModule *module, cSCPICmdInfo *scpicmdinfo)
    :m_pModule(module), m_pSCPICmdInfo(scpicmdinfo)
{
    m_bInitPending = false;

    m_measConfigureState.addTransition(this, SIGNAL(measContinue()), &m_measInitState);
    m_measInitState.addTransition(this, SIGNAL(measContinue()), &m_measFetchState);
    m_MeasureStateMachine.addState(&m_measConfigureState);
    m_MeasureStateMachine.addState(&m_measInitState);
    m_MeasureStateMachine.addState(&m_measFetchState);
    connect(&m_measConfigureState, SIGNAL(entered()), SLOT(configure()));
    connect(&m_measInitState, SIGNAL(entered()), SLOT(init()));
    connect(&m_measFetchState, SIGNAL(entered()), SLOT(fetch()));
    m_MeasureStateMachine.setInitialState(&m_measConfigureState);


    m_readInitState.addTransition(this, SIGNAL(measContinue()), &m_readFetchState);
    m_ReadStateMachine.addState(&m_readInitState);
    m_ReadStateMachine.addState(&m_readFetchState);
    connect(&m_readInitState, SIGNAL(entered()), SLOT(init()));
    connect(&m_readFetchState, SIGNAL(entered()), SLOT(fetch()));
    m_ReadStateMachine.setInitialState(&m_readInitState);


    m_initInitState.addTransition(this, SIGNAL(measContinue()), &m_initRdyState);
    m_InitStateMachine.addState(&m_initInitState);
    m_InitStateMachine.addState(&m_initRdyState);
    connect(&m_initInitState, SIGNAL(entered()), SLOT(init()));
    m_InitStateMachine.setInitialState(&m_initInitState);


    m_fetchWaitInitState.addTransition(this, SIGNAL(measContinue()), &m_fetchFetchState);
    m_FetchStateMachine.addState(&m_fetchWaitInitState);
    m_FetchStateMachine.addState(&m_fetchFetchState);
    connect(&m_fetchWaitInitState, SIGNAL(entered()), SLOT(waitInit()));
    connect(&m_fetchFetchState, SIGNAL(entered()), SLOT(fetch()));
    m_FetchStateMachine.setInitialState(&m_fetchWaitInitState);

}


cSCPIMeasure::~cSCPIMeasure()
{
    delete m_pSCPICmdInfo;
}


void cSCPIMeasure::execute(quint8 cmd)
{
    switch (cmd)
    {
    case SCPIModelType::measure:
        m_MeasureStateMachine.start();
        break;

    case SCPIModelType::configure:
        configuration();
        emit cmdStatus(SCPI::ack);
        break;

    case SCPIModelType::read:
        m_ReadStateMachine.start();
        break;

    case SCPIModelType::init:
        m_InitStateMachine.start();
        break;

    case SCPIModelType::fetch:
        m_FetchStateMachine.start();
        break;
    }
}


int cSCPIMeasure::entityID()
{
    return m_pSCPICmdInfo->entityId;
}


void cSCPIMeasure::configuration()
{
    // at the moment we have noting to do for configure
    emit measContinue(); // but if we are in statemachine we want to continue;
}


QString cSCPIMeasure::setAnswer(QVariant qvar)
{
    QString s;

    if (qvar.canConvert<QVariantList>())
    {
        QSequentialIterable iterable = qvar.value<QSequentialIterable>();

        s = QString("%1:%2:[%3]:").arg(m_pSCPICmdInfo->scpiModuleName, m_pSCPICmdInfo->scpiCommand, m_pSCPICmdInfo->unit);
        foreach (const QVariant &v, iterable)
            s += (v.toString()+",");
        s = s.remove(s.count()-1, 1);

    }
    else
        s = QString("%1:%2:[%3]:%4").arg(m_pSCPICmdInfo->scpiModuleName, m_pSCPICmdInfo->scpiCommand, m_pSCPICmdInfo->unit).arg(qvar.toString());

    return (s);
}


void cSCPIMeasure::configure()
{
    configuration();
}


void cSCPIMeasure::init()
{
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the initDone slot, so we synchronized on next measurement value
    m_pModule->scpiMeasureHash.insert(m_pSCPICmdInfo->componentName, this);
    m_bInitPending = true;
}


void cSCPIMeasure::waitInit()
{
    if (!m_bInitPending)
        emit measContinue();
}


void cSCPIMeasure::fetch()
{
    emit cmdAnswer(m_sAnswer);
}


void cSCPIMeasure::initDone(const QVariant qvar)
{
    m_sAnswer = setAnswer(qvar);
    m_bInitPending = false;
    emit measContinue();
}


}
