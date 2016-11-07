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
    m_ConfigureState.addTransition(this, SIGNAL(measContinue()), &m_InitState);
    m_InitState.addTransition(this, SIGNAL(measContinue()), &m_FetchState);

    m_MeasureStateMachine.addState(&m_ConfigureState);
    m_MeasureStateMachine.addState(&m_InitState);
    m_MeasureStateMachine.addState(&m_FetchState);

    connect(&m_ConfigureState, SIGNAL(entered()), SLOT(configure()));
    connect(&m_InitState, SIGNAL(entered()), SLOT(init()));
    connect(&m_FetchState, SIGNAL(entered()), SLOT(fetch()));
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
        initType = fromRead; // same as only from read because we have to output in this case
        m_MeasureStateMachine.setInitialState(&m_ConfigureState);
        m_MeasureStateMachine.start();
        break;

    case SCPIModelType::configure:
        configuration();
        emit cmdStatus(SCPI::ack);
        break;

    case SCPIModelType::read:
        initType = fromRead;
        m_MeasureStateMachine.setInitialState(&m_InitState);
        m_MeasureStateMachine.start();
        break;

    case SCPIModelType::init:
        initType = fromInit;
        m_MeasureStateMachine.setInitialState(&m_InitState);
        m_MeasureStateMachine.start();
        break;

    case SCPIModelType::fetch:
        emit cmdAnswer(m_sAnswer);
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
}


void cSCPIMeasure::fetch()
{
    if (initType == fromRead)
        emit cmdAnswer(m_sAnswer);
}


void cSCPIMeasure::initDone(const QVariant qvar)
{
    m_sAnswer = setAnswer(qvar);
    switch (initType)
    {
    case fromRead:
        emit measContinue(); // if we are in statemachine we want to continue;
        break;
    case fromInit:
        emit measContinue(); // so the statemachine will finish
        emit cmdStatus(SCPI::ack);
    }
}

}
