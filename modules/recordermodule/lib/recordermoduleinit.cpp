#include "recordermoduleinit.h"
#include "recordermoduleconfiguration.h"
#include "rpcreadrecordedvalues.h"
#include "recorderjsonexportveingethandler.h"
#include <boolvalidator.h>
#include <scpi.h>
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
    m_startStopRecording = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                 "PAR_StartStopRecording",
                                                 "Parameter to start/stop recording",
                                                 0);
    m_startStopRecording->setValidator(new cBoolValidator());
    m_startStopRecording->setScpiInfo("RECORDER", "RUN", SCPI::isQuery | SCPI::isCmdwP);
    m_module->m_veinModuleParameterMap[m_startStopRecording->getComponentName()] = m_startStopRecording;
    connect(m_startStopRecording, &VfModuleParameter::sigValueChanged, this, &RecorderModuleInit::startStopLogging);

    m_numberOfPointsInCurve = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                    "ACT_Points",
                                                    "Number of points in a curve",
                                                    0);
    m_numberOfPointsInCurve->setScpiInfo("RECORDER", "COUNT", SCPI::isQuery);
    m_module->m_veinModuleParameterMap[m_numberOfPointsInCurve->getComponentName()] = m_numberOfPointsInCurve;

    std::shared_ptr<RPCReadRecordedValues> rpcReadRecordedValues = std::make_shared<RPCReadRecordedValues>(m_module->getRpcEventSystem(),
                                                                                                           m_recorder->getRecordedData(),
                                                                                                           m_module->getEntityId());
    m_pReadRecordedValuesRpc = std::make_shared<VfModuleRpc>(rpcReadRecordedValues, "Read Recorded Values");
    m_module->m_veinModuleRPCMap[rpcReadRecordedValues->getSignature()] = m_pReadRecordedValuesRpc;

    m_jsonExportComponent = new VfModuleComponentStorageFetchOnly(m_module->getEntityId(),
                                                         "ACT_JSON_EXPORT",
                                                         "JSON export of recorded values",
                                                         m_module->getStorageDb());
    m_jsonExportComponent->setStorageGetCustomizer(std::make_shared<RecorderJsonExportVeinGetHandler>(m_recorder->getRecordedData()));
    m_jsonExportComponent->setScpiInfo("RECORDER", "EXPORT:JSON");
    m_module->m_veinComponentsWithMetaAndScpi.append(m_jsonExportComponent);
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
