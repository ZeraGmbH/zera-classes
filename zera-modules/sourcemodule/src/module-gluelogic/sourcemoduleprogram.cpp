#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <modulevalidator.h>
#include <intvalidator.h>
#include <jsonparamvalidator.h>
#include <vf-cpp-rpc.h>

#include "basedspmeasprogram.h"
#include "sourcemoduleprogram.h"
#include "sourcemodule.h"
#include "source-device/sourcedevicemanager.h"
#include "source-device/sourcedevicefacade.h"

SourceModuleProgram::SourceModuleProgram(SourceModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration),
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

void SourceModuleProgram::generateInterface()
{
    // source manager
    configuration* config = getConfigXMLWrapper();
    int maxSources = config->max_count_sources();
    m_pSourceDeviceManager = new SourceDeviceManager(maxSources);
    connect(m_pSourceDeviceManager, &SourceDeviceManager::sigAllSlotsRemoved,
            this, &SourceModuleProgram::sigLastSourceRemoved);
    connect(m_pSourceDeviceManager, &SourceDeviceManager::sigSourceScanFinished,
            this, &SourceModuleProgram::onSourceScanFinished, Qt::QueuedConnection);
    connect(m_pSourceDeviceManager, &SourceDeviceManager::sigSlotRemoved,
            this, &SourceModuleProgram::onSourceDeviceRemoved, Qt::QueuedConnection);

    // common components
    QString key;
    m_pVeinMaxCountAct = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                    QString("ACT_MaxSources"),
                                                    QString("Component with max source devices handled"),
                                                    QVariant(config->max_count_sources()) );
    m_pModule->veinModuleActvalueList.append(m_pVeinMaxCountAct); // auto delete / meta-data / scpi

    m_pVeinCountAct = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                    QString("ACT_CountSources"),
                                                    QString("Component containing count of active sources"),
                                                    QVariant(0) );
    m_pModule->veinModuleActvalueList.append(m_pVeinCountAct); // auto delete / meta-data / scpi

    m_pVeinDemoSourceCount = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                       key = QString("PAR_DemoSources"),
                                                       QString("Number of demo sources"),
                                                       QVariant(int(0)));
    m_pVeinDemoSourceCount->setValidator(new cIntValidator(0, maxSources));
    connect(m_pVeinDemoSourceCount, &cVeinModuleParameter::sigValueChanged, this, &SourceModuleProgram::newDemoSourceCount);
    m_pModule->veinModuleParameterHash[key] = m_pVeinDemoSourceCount; // auto delete / meta-data / scpi

    // RPCs
    m_sharedPtrRpcScanInterface = VfCpp::cVeinModuleRpc::Ptr(new VfCpp::cVeinModuleRpc(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             this, "RPC_ScanInterface",
                                             VfCpp::cVeinModuleRpc::Param({{"p_type", "int"},{"p_deviceInfo", "QString"}}),
                                             false, // !!! threaded on: signals do not reach theit slots
                                             false));
    m_pModule->veinModuleRpcList[m_sharedPtrRpcScanInterface->rpcName()] = m_sharedPtrRpcScanInterface; // for module's event handling

    m_sharedPtrRpcRemoveInterface = VfCpp::cVeinModuleRpc::Ptr(new VfCpp::cVeinModuleRpc(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             this, "RPC_CloseSource",
                                             VfCpp::cVeinModuleRpc::Param({{"p_deviceInfo", "QString"}}),
                                             false, // !!! threaded on: signals do not reach theit slots
                                             false));
    m_pModule->veinModuleRpcList[m_sharedPtrRpcRemoveInterface->rpcName()] = m_sharedPtrRpcRemoveInterface; // for module's event handling

    // per source components
    cVeinModuleActvalue* pVeinAct;
    cVeinModuleParameter* pVeinParam;
    cJsonParamValidator *jsonValidator;
    for(int souceCount=0; souceCount<maxSources; souceCount++) {
        SourceVeinInterface* sourceVeinInterface = new SourceVeinInterface;
        // device info (Don' movit down - our clients need it first!!)
        pVeinAct = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_DeviceInfo%1").arg(souceCount),
                                            QString("Component with source info/capabiliities"),
                                            QJsonObject());
        sourceVeinInterface->setVeinDeviceInfoComponent(pVeinAct);
        m_pModule->veinModuleActvalueList.append(pVeinAct); // auto delete / meta-data / scpi

        pVeinAct = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_DeviceState%1").arg(souceCount),
                                            QString("Component with source status"),
                                            QJsonObject());
        sourceVeinInterface->setVeinDeviceStateComponent(pVeinAct);
        m_pModule->veinModuleActvalueList.append(pVeinAct); // auto delete / meta-data / scpi

        // device param
        pVeinParam = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                            key = QString("PAR_SourceState%1").arg(souceCount),
                                                            QString("Component all source parameters in JSON format"),
                                                            QJsonObject());
        sourceVeinInterface->setVeinDeviceParameterComponent(pVeinParam);
        //pVeinParam->setSCPIInfo(new cSCPIInfo("CONFIGURATION","RANGE", "10", "PAR_NominalRange", "0", s));
        jsonValidator = new cJsonParamValidator();
        sourceVeinInterface->setVeinDeviceParameterValidator(jsonValidator);
        pVeinParam->setValidator(jsonValidator);
        m_pModule->veinModuleParameterHash[key] = pVeinParam; // auto delete / meta-data / scpi

        m_arrVeinIoInterfaces.append(sourceVeinInterface);
    }
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

configuration *SourceModuleProgram::getConfigXMLWrapper()
{
    return static_cast<SourceModuleConfiguration*>(m_pConfiguration.get())->getConfigXMLWrapper();
}

void SourceModuleProgram::updateDemoCount()
{
    m_bDeafenDemoChange = true;
    m_pVeinDemoSourceCount->setValue(m_pSourceDeviceManager->getDemoCount());
    m_bDeafenDemoChange = false;
}

void SourceModuleProgram::onSourceScanFinished(int slotPosition, QUuid uuid, QString errMsg)
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
                                               errMsg,
                                               sourceAdded);
}

void SourceModuleProgram::onSourceDeviceRemoved(int slot, QUuid uuid, QString errMsg)
{
    Q_UNUSED(slot);
    m_pVeinCountAct->setValue(QVariant(m_pSourceDeviceManager->getActiveSlotCount()));
    if(!uuid.isNull()) {
        bool ok = errMsg.isEmpty();
        m_sharedPtrRpcRemoveInterface->sendRpcResult(uuid,
                                                     ok ? VfCpp::cVeinModuleRpc::RPCResultCodes::RPC_SUCCESS : VfCpp::cVeinModuleRpc::RPCResultCodes::RPC_EINVAL,
                                                     errMsg,
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
