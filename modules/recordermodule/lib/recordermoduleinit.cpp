#include "recordermoduleinit.h"
#include "recordercsvexportveingethandler.h"
#include "recordermoduleconfiguration.h"
#include "rpcreadrecordedvalues.h"
#include "recorderjsonexportveingethandler.h"
#include <boolvalidator.h>
#include <scpi.h>
#include <timerfactoryqt.h>

using namespace VeinStorage;

RecorderModuleInit::RecorderModuleInit(RecorderModule *module,
                                       std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cModuleActivist(module->getVeinModuleName()),
    m_module(module),
    m_confData(qobject_cast<RecorderModuleConfiguration*>(pConfiguration.get())->getConfigurationData()),
    m_stopLoggingTimer(TimerFactoryQt::createSingleShot(m_confData->m_maxRecordingSeconds*1000))
{
    createRecorder();
    connect(m_recorder.get(), &StorageRecorder::sigRecordCountChanged, this, [=](int count) {
        m_numberOfPointsInCurve->setValue(count);
    });
    for(int i=0; i<m_confData->m_stackCount; i++) {
        connect(m_recorderInterpolationList.at(i).get(), &StorageRecorder::sigRecordCountChanged, this, [=](int count) {
            m_pointsList.at(i)->setValue(count);
        });
    }
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

    for(int i=0; i<m_confData->m_stackCount; i++) {
        VfModuleParameter* parameter = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                             QString("ACT_Points%1").arg(i+1),
                                                             "Number of points in a curve",
                                                             0);
        m_pointsList.append(parameter);
        m_module->m_veinModuleParameterMap[parameter->getComponentName()] = parameter;
    }

    std::shared_ptr<RPCReadRecordedValues> rpcReadRecordedValues = std::make_shared<RPCReadRecordedValues>(m_module->getRpcEventSystem(),
                                                                                                           m_recorder->getRecordedData(),
                                                                                                           "RPC_ReadRecordedValues",
                                                                                                           m_module->getEntityId());
    m_pReadRecordedValuesRpc = std::make_shared<VfModuleRpc>(rpcReadRecordedValues, "Read Recorded Values");
    m_module->m_veinModuleRPCMap[rpcReadRecordedValues->getSignature()] = m_pReadRecordedValuesRpc;

    for(int i=0; i<m_confData->m_stackCount; i++) {
        std::shared_ptr<RPCReadRecordedValues> rpcRecordedSample = std::make_shared<RPCReadRecordedValues>(m_module->getRpcEventSystem(),
                                                                                                                   m_recorderInterpolationList.at(i)->getRecordedData(),
                                                                                                                   QString("RPC_GetRecordedValuesReduced%1").arg(i+1),
                                                                                                                   m_module->getEntityId());
        m_pGetRecordedSampleValuesRpc = std::make_shared<VfModuleRpc>(rpcRecordedSample, "Get Recorded Values sample");
        m_module->m_veinModuleRPCMap[rpcRecordedSample->getSignature()] = m_pGetRecordedSampleValuesRpc;
    }

    m_jsonExportComponent = new VfModuleComponentStorageFetchOnly(m_module->getEntityId(),
                                                         "ACT_JSON_EXPORT",
                                                         "JSON export of recorded values",
                                                         m_module->getStorageDb());
    m_jsonExportComponent->setStorageGetCustomizer(std::make_shared<RecorderJsonExportVeinGetHandler>(m_recorder->getRecordedData()));
    m_jsonExportComponent->setScpiInfo("RECORDER", "EXPORT:JSON");
    m_module->m_veinComponentsWithMetaAndScpi.append(m_jsonExportComponent);

    m_csvExportComponent = new VfModuleComponentStorageFetchOnly(m_module->getEntityId(),
                                                                 "ACT_CSV_EXPORT",
                                                                 "CSV export of recorded values",
                                                                 m_module->getStorageDb());
    m_csvExportComponent->setStorageGetCustomizer(std::make_shared<RecorderCsvExportVeinGetHandler>(m_recorder->getRecordedData()));
    m_csvExportComponent->setScpiInfo("RECORDER", "EXPORT:CSV");
    m_module->m_veinComponentsWithMetaAndScpi.append(m_csvExportComponent);
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
    m_recorder = std::make_shared<StorageRecorder>(entityComponents,
                                                   m_module->getStorageDb(),
                                                   entityIdDftModule, "SIG_Measuring");

    for(int i=0; i<m_confData->m_stackCount; i++) {
        int factor = m_confData->m_valueLengthVector[i];
        std::shared_ptr<StorageRecorderInterpolation> recorderInterpolation;
        if(m_recorderInterpolationList.isEmpty())
            recorderInterpolation = std::make_shared<StorageRecorderInterpolation>(entityComponents, m_recorder, factor);
        else
            recorderInterpolation = std::make_shared<StorageRecorderInterpolation>(entityComponents, m_recorderInterpolationList[i-1], factor);
        m_recorderInterpolationList.append(recorderInterpolation);
    }
}
