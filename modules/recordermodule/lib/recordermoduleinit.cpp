#include "recordermoduleinit.h"
#include "recordermoduleconfiguration.h"
#include "rpcreadrecordedvalues.h"
#include <boolvalidator.h>
#include <timerfactoryqt.h>

static int constexpr timer = 1200000; // 20 mins

using namespace VeinStorage;

RecorderModuleInit::RecorderModuleInit(RecorderModule *module,
                                       std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cModuleActivist(module->getVeinModuleName()),
    m_module(module),
    m_confData(qobject_cast<RecorderModuleConfiguration*>(pConfiguration.get())->getConfigurationData()),
    m_stopLoggingTimer(TimerFactoryQt::createSingleShot(timer))
{
    createRecorder();
    connect(m_recorder.get(), &StorageRecorder::sigRecordCountChanged, this, [=](int count) {
        m_numberOfPointsInCurve->setValue(count);
    });
    connect(m_stopLoggingTimer.get(), &TimerTemplateQt::sigExpired, this, [this]() {
        m_startStopRecording->setValue(false); // Does not emit sigValueChanged
        startStopLogging(false);
    });
}

void RecorderModuleInit::activate()
{
    emit activated();
}

void RecorderModuleInit::deactivate()
{
    emit deactivated();
}

void RecorderModuleInit::generateVeinInterface()
{
    // SCPI cmd ?
    VfRpcEventSystemSimplified *rpcEventSystem = m_module->getRpcEventSystem();

    std::shared_ptr<RPCReadRecordedValues> rpcReadRecordedValues = std::make_shared<RPCReadRecordedValues>(rpcEventSystem,
                                                                                                           m_recorder->getRecordedData(),
                                                                                                           m_module->getEntityId());
    m_pReadRecordedValuesRpc = std::make_shared<VfModuleRpc>(rpcReadRecordedValues,
                                                         "Read Recorded Values");
    m_module->m_veinModuleRPCMap[rpcReadRecordedValues->getSignature()] = m_pReadRecordedValuesRpc; // for modules use

    m_numberOfPointsInCurve = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                    QString("ACT_Points"),
                                                    QString("Number of points in a curve"),
                                                    QVariant(0));

    m_module->m_veinComponentsWithMetaAndScpi.append(m_numberOfPointsInCurve); // we add the component for the modules interface

    QString key;
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
    if(startStopRecording.toBool()) {
        m_stopLoggingTimer->start();
        m_recorder->startLogging();
    }
    else
        m_recorder->stopLogging();
}

void RecorderModuleInit::createRecorder()
{
    RecordEntityComponents entityComponents;
    for (int entityNo=0; entityNo<m_confData->m_entityCount; ++entityNo) {
        QList<RecordComponent> components;
        for (int componentNo=0; componentNo<m_confData->m_entityConfigList[entityNo].m_components.count(); ++componentNo) {
            const ComponentConfiguration componentConf = m_confData->m_entityConfigList[entityNo].m_components[componentNo];
            components.append({componentConf.m_componentName, componentConf.m_label});
        }
        int entityId = m_confData->m_entityConfigList[entityNo].m_entityId;
        entityComponents[entityId] = components;
    }
    static constexpr int entityIdDftModule = 1050;
    m_recorder = std::make_unique<StorageRecorder>(entityComponents,
                                                   m_module->getStorageDb(),
                                                   entityIdDftModule, "SIG_Measuring");
}
