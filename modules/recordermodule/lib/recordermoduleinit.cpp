#include "recordermoduleinit.h"
#include <boolvalidator.h>

RecorderModuleInit::RecorderModuleInit(RecorderModule *module) :
    cModuleActivist(module->getVeinModuleName()),
    m_module(module)
{
}

void RecorderModuleInit::activate()
{
    emit m_module->activationContinue();
}

void RecorderModuleInit::deactivate()
{
    emit m_module->deactivationContinue();
}

void RecorderModuleInit::generateVeinInterface()
{
    // SCPI cmd ?
    QString key;
    VfRpcEventSystemSimplified *rpcEventSystem = m_module->getRpcEventSystem();

    m_numberOfPointsInCurve = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                 QString("ACT_Points"),
                                                 QString("Number of points in a curve"));

    m_module->veinModuleActvalueList.append(m_numberOfPointsInCurve); // we add the component for the modules interface

    m_startStopRecording = new VfModuleParameter(m_module->getEntityId(),
                                                 m_module->getValidatorEventSystem(),
                                                 key = QString("PAR_StartStopRecording"),
                                                 QString("Parameter to start/stop recording"),
                                                 QVariant(false));
    m_startStopRecording->setValidator(new cBoolValidator());
    //SCPI ?
    m_module->m_veinModuleParameterMap[key] = m_startStopRecording; // for modules use
}
