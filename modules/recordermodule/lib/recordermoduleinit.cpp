#include "recordermoduleinit.h"
#include "rpcreadrecordedvalues.h"
#include <boolvalidator.h>

RecorderModuleInit::RecorderModuleInit(RecorderModule *module) :
    cModuleActivist(module->getVeinModuleName()),
    m_module(module)
{
    m_dataCollector = std::make_shared<VeinDataCollector>(m_module->getStorageDb());
    connect(m_dataCollector.get(), &VeinDataCollector::newValueStored, this, &RecorderModuleInit::setNumberOfPointsInCurve);
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

    std::shared_ptr<RPCReadRecordedValues> rpcReadRecordedValues = std::make_shared<RPCReadRecordedValues>(rpcEventSystem, m_dataCollector,m_module->getEntityId());
    m_pReadRecordedValuesRpc = std::make_shared<VfModuleRpc>(rpcReadRecordedValues,
                                                         "Read Recorded Values");
    m_module->m_veinModuleRPCMap[rpcReadRecordedValues->getSignature()] = m_pReadRecordedValuesRpc; // for modules use

    m_numberOfPointsInCurve = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                    QString("ACT_Points"),
                                                    QString("Number of points in a curve"),
                                                    QVariant(0));

    m_module->veinModuleActvalueList.append(m_numberOfPointsInCurve); // we add the component for the modules interface

    m_startStopRecording = new VfModuleParameter(m_module->getEntityId(),
                                                 m_module->getValidatorEventSystem(),
                                                 key = QString("PAR_StartStopRecording"),
                                                 QString("Parameter to start/stop recording"),
                                                 QVariant(false));
    m_startStopRecording->setValidator(new cBoolValidator());
    //SCPI ?
    m_module->m_veinModuleParameterMap[key] = m_startStopRecording; // for modules use

    connect(m_startStopRecording, &VfModuleParameter::sigValueChanged, this, &RecorderModuleInit::startStopLogging);
}

void RecorderModuleInit::startStopLogging(QVariant startStopRecording)
{
    QHash<int, QStringList> entitesAndComponents;
    QStringList rmscomponents {"ACT_RMSPN1", "ACT_RMSPN2", "ACT_RMSPN3", "ACT_RMSPN4", "ACT_RMSPN5", "ACT_RMSPN6"};
    entitesAndComponents.insert(1040, rmscomponents);
    QStringList powerComponents {"ACT_PQS1", "ACT_PQS2", "ACT_PQS3", "ACT_PQS4"};
    entitesAndComponents.insert(1070, powerComponents);

    if(startStopRecording.toBool()) {
        setNumberOfPointsInCurve(0);
        m_dataCollector->startLogging(entitesAndComponents);
    }
    else
        m_dataCollector->stopLogging();
}

void RecorderModuleInit::setNumberOfPointsInCurve(int num)
{
    m_numberOfPointsInCurve->setValue(num);
}
