#include <veinentity.h>
#include <scpi.h>
#include "scpimeasure.h"
#include "scpiinterface.h"


namespace SCPIMODULE
{

cSCPIMeasure::cSCPIMeasure(VeinEntity* entity)
    :m_pEntity(entity)
{
    m_ConfigureState.addTransition(this, SIGNAL(measContinue()), &m_InitState);
    m_InitState.addTransition(this, SIGNAL(measContinue()), &m_FetchState);

    m_MeasureStateMachine.addState(&m_ConfigureState);
    m_MeasureStateMachine.addState(&m_InitState);
    m_MeasureStateMachine.addState(&m_FetchState);

    connect(&m_ConfigureState, SIGNAL(entered()), SLOT(configure()));
    connect(&m_InitState, SIGNAL(entered()), SLOT(init()));
    connect(&m_FetchState, SIGNAL(entered()), SLOT(fetch()));


}


void cSCPIMeasure::execute(quint8 cmd)
{
    switch (cmd)
    {
    case SCPIModelType::measure:
        m_MeasureStateMachine.setInitialState(&m_ConfigureState);
        m_MeasureStateMachine.start();
        break;

    case SCPIModelType::configure:
        configuration();
        emit cmdStatus(SCPI::ack);
        break;

    case SCPIModelType::read:
        m_MeasureStateMachine.setInitialState(&m_InitState);
        m_MeasureStateMachine.start();
        break;

    case SCPIModelType::init:
        connect(m_pEntity, SIGNAL(sigValueChanged(const QVariant)), this, SLOT(initCmdDone(const QVariant)));
        break;

    case SCPIModelType::fetch:
        emit cmdAnswer(m_sAnswer);
        break;
    }
}


void cSCPIMeasure::configuration()
{
    // at the moment we have noting to do for configure
    emit measContinue(); // but if we are in statemachine we want to continue;
}


void cSCPIMeasure::configure()
{
    configuration();
}


void cSCPIMeasure::init()
{
    connect(m_pEntity, SIGNAL(sigvalueChanged(const QVariant)), this, SLOT(initDone(const QVariant)));
}


void cSCPIMeasure::fetch()
{
    emit cmdAnswer(m_sAnswer);
}


void cSCPIMeasure::initDone(const QVariant qvar)
{
    m_sAnswer = qvar.toString();
    disconnect(m_pEntity, SIGNAL(sigvalueChanged()), this, SLOT(initDone()));
    emit measContinue(); // if we are in statemachine we want to continue;
}


void cSCPIMeasure::initCmdDone(const QVariant qvar)
{
    m_sAnswer = qvar.toString();
    disconnect(m_pEntity, SIGNAL(sigvalueChanged(const QVariant)), this, SLOT(sigvalueChanged(const QVariant)));
    emit cmdStatus(SCPI::ack);
}

}
