#include "recordermoduleinit.h"
#include "rpcreadrecordedvalues.h"
#include <boolvalidator.h>
#include <timerfactoryqt.h>

static int constexpr systemEntityId = 0;
static int constexpr rmsEntityId = 1040;
static int constexpr fftEntityId = 1060;
static int constexpr powerAcEntityId = 1070;
static int constexpr powerDcEntityId = 1073;
static int constexpr timer = 1200000; // 20 mins

RecorderModuleInit::RecorderModuleInit(RecorderModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cModuleActivist(module->getVeinModuleName()),
    m_module(module)
{
    m_stopLoggingTimer = TimerFactoryQt::createSingleShot(timer);
    connect(m_stopLoggingTimer.get(), &TimerTemplateQt::sigExpired, this, [this]() {
        m_startStopRecording->setValue(false); // Does not emit sigValueChanged
        startStopLogging(false);
    });

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

QHash<int, QStringList> RecorderModuleInit::setEntitiesAndComponentsToRecord()
{
    QHash<int, QStringList> entitesAndComponents;
    VeinStorage::AbstractComponentPtr sessionName = m_module->getStorageDb()->findComponent(systemEntityId, "Session");

    if(sessionName->getValue().toString().contains("mt310s2-emob-session-ac")) {
        QStringList rmsComponents {"ACT_RMSPN1", "ACT_RMSPN2", "ACT_RMSPN3", "ACT_RMSPN4", "ACT_RMSPN5", "ACT_RMSPN6"};
        entitesAndComponents.insert(rmsEntityId, rmsComponents);
        QStringList powerComponents {"ACT_PQS1", "ACT_PQS2", "ACT_PQS3", "ACT_PQS4"};
        entitesAndComponents.insert(powerAcEntityId, powerComponents);
    }
    else if(sessionName->getValue().toString().contains("mt310s2-emob-session-dc")) {
        QStringList fftComponents {"ACT_DC7", "ACT_DC8"};
        entitesAndComponents.insert(fftEntityId, fftComponents);
        QStringList powerComponents {"ACT_PQS1"};
        entitesAndComponents.insert(powerDcEntityId, powerComponents);
    }
    return entitesAndComponents;
}

void RecorderModuleInit::startStopLogging(QVariant startStopRecording)
{
    QHash<int, QStringList> entitesAndComponents = setEntitiesAndComponentsToRecord();
    if(startStopRecording.toBool()) {
        m_stopLoggingTimer->start();
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
