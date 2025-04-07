#include "sourcemoduleprogram.h"
#include "sourcemodule.h"
#include "sourcedevicemanager.h"
#include "sourcedevicefacade.h"
#include <vfmoduleparameter.h>
#include <intvalidator.h>
#include <jsonparamvalidator.h>

SourceModuleProgram::SourceModuleProgram(SourceModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module)
{
}

SourceModuleProgram::~SourceModuleProgram()
{
    delete m_pSourceDeviceManager;
    while(m_arrVeinIoInterfaces.size()) {
        delete m_arrVeinIoInterfaces.takeLast();
    }
}

void SourceModuleProgram::generateVeinInterface()
{
    // source manager
    int maxSources = 4;
    m_pSourceDeviceManager = new SourceDeviceManager(maxSources);
    connect(m_pSourceDeviceManager, &SourceDeviceManager::sigAllSlotsRemoved,
            this, &SourceModuleProgram::sigLastSourceRemoved);
    connect(m_pSourceDeviceManager, &SourceDeviceManager::sigSourceScanFinished,
            this, &SourceModuleProgram::onSourceScanFinished, Qt::QueuedConnection);
    connect(m_pSourceDeviceManager, &SourceDeviceManager::sigSlotRemoved,
            this, &SourceModuleProgram::onSourceDeviceRemoved, Qt::QueuedConnection);

    // common components
    QString key;
    m_pVeinMaxCountAct = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                    QString("ACT_MaxSources"),
                                                    QString("Max source devices"),
                                                    QVariant(maxSources) );
    m_pModule->veinModuleActvalueList.append(m_pVeinMaxCountAct); // auto delete / meta-data / scpi

    m_pVeinCountAct = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                    QString("ACT_CountSources"),
                                                    QString("Active source count"),
                                                    QVariant(0) );
    m_pModule->veinModuleActvalueList.append(m_pVeinCountAct); // auto delete / meta-data / scpi

    // RPCs
    m_sharedPtrRpcScanInterface = VfCpp::cVeinModuleRpc::Ptr(new VfCpp::cVeinModuleRpc(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             this, "RPC_ScanInterface",
                                             VfCpp::cVeinModuleRpc::Param({{"p_type", "int"},{"p_deviceInfo", "QString"}}),
                                             false, // !!! threaded on: signals do not reach theit slots
                                             false));
    m_pModule->getRpcEventSystem()->addRpc(m_sharedPtrRpcScanInterface);

    m_sharedPtrRpcRemoveInterface = VfCpp::cVeinModuleRpc::Ptr(new VfCpp::cVeinModuleRpc(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                             this, "RPC_CloseSource",
                                             VfCpp::cVeinModuleRpc::Param({{"p_deviceInfo", "QString"}}),
                                             false, // !!! threaded on: signals do not reach theit slots
                                             false));
    m_pModule->getRpcEventSystem()->addRpc(m_sharedPtrRpcRemoveInterface);

    // per source components
    VfModuleComponent* pVeinAct;
    VfModuleParameter* pVeinParam;
    cJsonParamValidator *jsonValidator;
    for(int souceCount=0; souceCount<maxSources; souceCount++) {
        SourceVeinInterface* sourceVeinInterface = new SourceVeinInterface;
        // device info (Don' movit down - our clients need it first!!)
        pVeinAct = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_DeviceInfo%1").arg(souceCount),
                                            QString("Source info/capabiliities in JSON"),
                                            QJsonObject());
        sourceVeinInterface->setVeinDeviceInfoComponent(pVeinAct);
        m_pModule->veinModuleActvalueList.append(pVeinAct); // auto delete / meta-data / scpi

        pVeinAct = new VfModuleComponent(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                            QString("ACT_DeviceState%1").arg(souceCount),
                                            QString("Source status in JSON"),
                                            QJsonObject());
        sourceVeinInterface->setVeinDeviceStateComponent(pVeinAct);
        m_pModule->veinModuleActvalueList.append(pVeinAct); // auto delete / meta-data / scpi

        // device param
        pVeinParam = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                            key = QString("PAR_SourceState%1").arg(souceCount),
                                                            QString("All source parameters in JSON"),
                                                            QJsonObject());
        sourceVeinInterface->setVeinDeviceParameterComponent(pVeinParam);
        jsonValidator = new cJsonParamValidator();
        sourceVeinInterface->setVeinDeviceParameterValidator(jsonValidator);
        pVeinParam->setValidator(jsonValidator);
        m_pModule->m_veinModuleParameterMap[key] = pVeinParam; // auto delete / meta-data / scpi

        m_arrVeinIoInterfaces.append(sourceVeinInterface);
    }
    m_pVeinDemoSourceCount = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                                   key = QString("PAR_DemoSources"),
                                                   QString("Number of demo sources"),
                                                   QVariant(int(0)));
    m_pVeinDemoSourceCount->setValidator(new cIntValidator(0, maxSources));
    connect(m_pVeinDemoSourceCount, &VfModuleParameter::sigValueChanged, this, &SourceModuleProgram::newDemoSourceCount);
    m_pModule->m_veinModuleParameterMap[key] = m_pVeinDemoSourceCount; // auto delete / meta-data / scpi
    newDemoSourceCount(QVariant(m_pModule->getDemo() ? maxSources : 0));
}

void SourceModuleProgram::startDestroy()
{
    m_pSourceDeviceManager->closeAll();
}

QVariant SourceModuleProgram::RPC_ScanInterface(QVariantMap p_params)
{
    int ioDeviceType = p_params["p_type"].toInt();
    QString deviceInfo = p_params["p_deviceInfo"].toString();
    QUuid veinUuid = p_params[VeinComponent::RemoteProcedureData::s_callIdString].toUuid();
    m_pSourceDeviceManager->startSourceScan(IoDeviceTypes(ioDeviceType), deviceInfo, veinUuid);
    return true;
}

QVariant SourceModuleProgram::RPC_CloseSource(QVariantMap p_params)
{
    QString deviceInfo = p_params["p_deviceInfo"].toString();
    QUuid uuid = p_params[VeinComponent::RemoteProcedureData::s_callIdString].toUuid();
    m_pSourceDeviceManager->closeSource(deviceInfo, uuid);
    return true;
}

void SourceModuleProgram::updateDemoCount()
{
    m_bDeafenDemoChange = true;
    m_pVeinDemoSourceCount->setValue(m_pSourceDeviceManager->getDemoCount());
    m_bDeafenDemoChange = false;
}

void SourceModuleProgram::onSourceScanFinished(int slotPosition, QUuid uuid, QString errorMsg)
{
    bool sourceAdded = slotPosition >= 0;
    if(sourceAdded) {
        SourceDeviceFacade::Ptr deviceContainer = m_pSourceDeviceManager->getSourceController(slotPosition);
        deviceContainer->setVeinInterface(m_arrVeinIoInterfaces[slotPosition]);
        m_pVeinCountAct->setValue(QVariant(m_pSourceDeviceManager->getActiveSlotCount()));
        updateDemoCount();
    }
    m_sharedPtrRpcScanInterface->sendRpcResult(uuid,
                                               sourceAdded ? VfCpp::cVeinModuleRpc::RPCResultCodes::RPC_SUCCESS : VfCpp::cVeinModuleRpc::RPCResultCodes::RPC_EINVAL,
                                               errorMsg,
                                               sourceAdded);
}

void SourceModuleProgram::onSourceDeviceRemoved(int slot, QUuid uuid, QString errorMsg)
{
    Q_UNUSED(slot);
    m_pVeinCountAct->setValue(QVariant(m_pSourceDeviceManager->getActiveSlotCount()));
    if(!uuid.isNull()) {
        bool ok = errorMsg.isEmpty();
        m_sharedPtrRpcRemoveInterface->sendRpcResult(uuid,
                                                     ok ? VfCpp::cVeinModuleRpc::RPCResultCodes::RPC_SUCCESS : VfCpp::cVeinModuleRpc::RPCResultCodes::RPC_EINVAL,
                                                     errorMsg,
                                                     ok);
    }
    updateDemoCount();
}

void SourceModuleProgram::newDemoSourceCount(QVariant getDemoCount)
{
    if(!m_bDeafenDemoChange) {
        m_pSourceDeviceManager->setDemoCount(getDemoCount.toInt());
    }
}
