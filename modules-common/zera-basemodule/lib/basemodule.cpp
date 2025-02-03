#include "basemodule.h"
#include "vfmodulecomponent.h"
#include "vf_server_entity_add.h"
#include "vf_server_entity_remove.h"
#include "taskactivationstatemachinewrapper.h"
#include <tasklambdarunner.h>
#include <taskcontainersequence.h>
#include <QJsonArray>
#include <QJsonDocument>

BaseModule::BaseModule(ModuleFactoryParam moduleParam, std::shared_ptr<BaseModuleConfiguration> modcfg) :
    ZeraModules::VirtualModule(moduleParam.m_moduleNum, moduleParam.m_entityId),
    m_pConfiguration(modcfg),
    m_moduleParam(moduleParam)
{
    m_pConfiguration->setConfiguration(moduleParam.m_configXmlData);
    if(!m_pConfiguration->isConfigured()) {
        qCritical("Module config incomplete on module entity ID %i / Num %i", moduleParam.m_entityId, moduleParam.m_moduleNum);
        return;
    }

    // now we set up our machines for activating, deactivating a module
    m_ActivationStartState.addTransition(this, &BaseModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &BaseModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &BaseModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &BaseModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &BaseModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &BaseModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &BaseModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &BaseModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &BaseModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &BaseModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &BaseModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &BaseModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &BaseModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &BaseModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &BaseModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &BaseModule::deactivationFinished);
}

BaseModule::~BaseModule()
{
    emit m_pModuleEventSystem->sigSendEvent(VfServerEntityRemove::generateEvent(getEntityId()));
    unsetModule();
}

void BaseModule::startSetupTask()
{
    TaskContainerInterfacePtr startTasks = TaskContainerSequence::create();
    startTasks->addSub(getModuleSetUpTask());
    startTasks->addSub(TaskLambdaRunner::create([this]() {
            setupModule();
            return true;
        },
        true));

    TaskActivationStateMachineWrapperPtr activationStatemachineTask = TaskActivationStateMachineWrapper::create(&m_ActivationMachine);
    connect(this, &BaseModule::activationReady,
            activationStatemachineTask.get(), &TaskActivationStateMachineWrapper::onActivationFinished);
    startTasks->addSub(std::move(activationStatemachineTask));

    startTasks->addSub(TaskLambdaRunner::create([this]() {
            startMeas();
            m_moduleIsFullySetUp = true;
            emit moduleActivated();
            return true;
        },
        true));
    m_startupTask = std::move(startTasks);
    m_startupTask->start();
}

void BaseModule::startModule()
{
    if(m_moduleIsFullySetUp)
        startMeas();
    else
        startSetupTask();
}

void BaseModule::stopModule()
{
    if(m_moduleIsFullySetUp)
        stopMeas();
    else
        qCritical("Stopping a module not set up is not (yet?) supported!");
}

void BaseModule::startDestroy()
{
    connect(this, &BaseModule::deactivationReady,
            this, &ZeraModules::VirtualModule::moduleDeactivated);
    if(!m_DeactivationMachine.isRunning())
        m_DeactivationMachine.start();
}

void BaseModule::setupModule()
{
    emit m_pModuleEventSystem->sigSendEvent(VfServerEntityAdd::generateEvent(getEntityId()));

    m_pModuleEntityName = new VfModuleComponent(getEntityId(), m_pModuleEventSystem,
                                                   QString("EntityName"),
                                                   QString("Module's name"),
                                                   QVariant(m_sModuleName));

    veinModuleComponentList.append(m_pModuleEntityName);

    m_pModuleInterfaceComponent = new VfModuleComponent(getEntityId(), m_pModuleEventSystem,
                                                           QString("INF_ModuleInterface"),
                                                           QString("Module's interface details"),
                                                           QByteArray());

    veinModuleComponentList.append(m_pModuleInterfaceComponent);

    m_pModuleName = new VfModuleMetaData(QString("Name"), QVariant(m_sModuleName));
    veinModuleMetaDataList.append(m_pModuleName);

    m_pModuleDescription = new VfModuleMetaData(QString("Description"), QVariant(m_sModuleDescription));
    veinModuleMetaDataList.append(m_pModuleDescription);
}

void BaseModule::unsetModule()
{
    for (auto veinModuleMetaData : qAsConst(veinModuleMetaDataList))
        delete veinModuleMetaData;
    veinModuleMetaDataList.clear();

    for (auto veinModuleComponent : qAsConst(veinModuleComponentList))
        delete veinModuleComponent;
    veinModuleComponentList.clear();

    for (auto veinModuleActvalue : qAsConst(veinModuleActvalueList))
        delete veinModuleActvalue;
    veinModuleActvalueList.clear();

    for (auto scpiCommand : qAsConst(scpiCommandList))
        delete scpiCommand;
    scpiCommandList.clear();

    for (auto veinModuleParameter : qAsConst(m_veinModuleParameterMap))
        delete veinModuleParameter;
    m_veinModuleParameterMap.clear();

    for (auto ModuleActivist : qAsConst(m_ModuleActivistList))
        delete ModuleActivist;
    m_ModuleActivistList.clear();
}

QByteArray BaseModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}

void BaseModule::activationStart()
{
    // Interim - we intend to replace all this by tasks
    if(!m_ModuleActivistList.isEmpty()) {
        m_nActivationIt = 0; // we start with the first
        emit activationContinue();
    }
}

void BaseModule::activationExec()
{
    // Interim - we intend to replace all this by tasks
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}

void BaseModule::activationDone()
{
    m_nActivationIt++;
    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}

void BaseModule::deactivationStart()
{
    // Interim - we intend to replace all this by tasks
    if(!m_ModuleActivistList.isEmpty()) {
        m_nActivationIt = 0; // we start with the first
        emit deactivationContinue();
    }
}

void BaseModule::deactivationExec()
{
    // Interim - we intend to replace all this by tasks
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}

void BaseModule::deactivationDone()
{
    m_nActivationIt++;
    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}

void BaseModule::deactivationFinished()
{
    emit deactivationReady();
}

TaskTemplatePtr BaseModule::getModuleSetUpTask()
{
    return TaskLambdaRunner::create([]() {
        return true;
    });
}

void BaseModule::exportMetaData()
{
    QJsonObject jsonObj;
    QJsonObject jsonObj2;

    for (int i = 0; i < veinModuleMetaDataList.count(); i++)
        veinModuleMetaDataList.at(i)->exportMetaData(jsonObj2);
    jsonObj.insert("ModuleInfo", jsonObj2);

    QJsonObject jsonObj3;
    for (int i = 0; i < veinModuleComponentList.count(); i++)
        veinModuleComponentList.at(i)->exportMetaData(jsonObj3);
    for (int i = 0; i < veinModuleActvalueList.count(); i++)
        veinModuleActvalueList.at(i)->exportMetaData(jsonObj3);

    QList<QString> keyList;
    keyList = m_veinModuleParameterMap.keys();
    for (int i = 0; i < keyList.count(); i++)
        m_veinModuleParameterMap[keyList.at(i)]->exportMetaData(jsonObj3);
    jsonObj.insert("ComponentInfo", jsonObj3);

    QJsonArray jsonArr;
    // and then all the command information for actual values, parameters and for add. commands without components
    for (int i = 0; i < scpiCommandList.count(); i++)
        scpiCommandList.at(i)->appendSCPIInfo(jsonArr);
    for (int i = 0; i < veinModuleActvalueList.count(); i++)
        veinModuleActvalueList.at(i)->exportSCPIInfo(jsonArr);

    for (int i = 0; i < keyList.count(); i++)
        m_veinModuleParameterMap[keyList.at(i)]->exportSCPIInfo(jsonArr);

    QJsonObject jsonObj4;
    jsonObj4.insert("Name", m_sSCPIModuleName);
    jsonObj4.insert("Cmd", jsonArr);
    jsonObj.insert("SCPIInfo", jsonObj4);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    QByteArray ba;
    ba = jsonDoc.toJson();

    m_pModuleInterfaceComponent->setValue(QVariant(ba));
}

VeinStorage::AbstractDatabase *BaseModule::getStorageDb() const
{
    return m_moduleParam.m_storagesystem->getDb();
}

bool BaseModule::getDemo()
{
    return m_moduleParam.m_moduleSharedData->m_demo;
}

const AbstractFactoryServiceInterfacesPtr BaseModule::getServiceInterfaceFactory() const
{
    return m_moduleParam.m_moduleSharedData->m_serviceInterfaceFactory;
}

const ModuleNetworkParamsPtr BaseModule::getNetworkConfig() const
{
    return m_moduleParam.m_moduleSharedData->m_networkParams;
}

const ChannelRangeObserver::SystemObserverPtr BaseModule::getSharedChannelRangeObserver() const
{
    return m_moduleParam.m_moduleSharedData->m_channelRangeObserver;
}
