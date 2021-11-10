#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <modulevalidator.h>
#include <intvalidator.h>
#include <jsonparamvalidator.h>
#include <vf-cpp-rpc.h>

#include "basedspmeasprogram.h"
#include "sourcemoduleprogram.h"
#include "sourcemodule.h"
#include "sourcedevicemanager.h"
#include "sourcedevice.h"
#include "sourceveininterface.h"
#include "sourceinterface.h"

namespace SOURCEMODULE
{

cSourceModuleProgram::cSourceModuleProgram(cSourceModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration) :
    cBaseMeasWorkProgram(pConfiguration),
    m_pSourceDeviceManager(nullptr),
    m_pModule(module)
{
}

cSourceModuleProgram::~cSourceModuleProgram()
{
    delete m_pSourceDeviceManager;
    while(m_arrVeinSourceInterfaces.size()) {
        delete m_arrVeinSourceInterfaces.takeLast();
    }
}

void cSourceModuleProgram::generateInterface()
{
    // source manager
    configuration* config = getConfigXMLWrapper();
    int maxSources = config->max_count_sources();
    m_pSourceDeviceManager = new cSourceDeviceManager(maxSources);
    connect(m_pSourceDeviceManager, &cSourceDeviceManager::sigSourceScanFinished,
            this, &cSourceModuleProgram::onSourceScanFinished, Qt::QueuedConnection);
    connect(m_pSourceDeviceManager, &cSourceDeviceManager::sigSlotRemoved,
            this, &cSourceModuleProgram::onSourceDeviceRemoved, Qt::QueuedConnection);

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
                                                       QString("Component for setting number of demo sources"),
                                                       QVariant(int(0)));
    m_pVeinDemoSourceCount->setValidator(new cIntValidator(0, maxSources));
    connect(m_pVeinDemoSourceCount, &cVeinModuleParameter::sigValueChanged, this, &cSourceModuleProgram::newDemoSourceCount);
    m_pModule->veinModuleParameterHash[key] = m_pVeinDemoSourceCount; // auto delete / meta-data / scpi

    // RPC
    m_sharedPtrRpcScanInterface = VfCpp::cVeinModuleRpc::Ptr(new VfCpp::cVeinModuleRpc(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             this, "RPC_ScanInterface",
                                             VfCpp::cVeinModuleRpc::Param({{"p_type", "int"},{"p_deviceInfo", "QString"}}),
                                             false, // !!! threaded on: signals do not reach theit slots
                                             false));
    m_pModule->veinModuleRpcList[m_sharedPtrRpcScanInterface->rpcName()] = m_sharedPtrRpcScanInterface; // for module's event handling

    // per source components
    cVeinModuleActvalue* pVeinAct;
    cVeinModuleParameter* pVeinParam;
    cJsonParamValidator *jsonValidator;
    for(int souceCount=0; souceCount<maxSources; souceCount++) {
        cSourceVeinInterface* sourceVeinInterface = new cSourceVeinInterface;
        // device info (Don' movit down - our clients need it first!!)
        pVeinAct = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_DeviceInfo%1").arg(souceCount),
                                            QString("Component with source info/capabiliities"),
                                            QJsonObject());
        sourceVeinInterface->setVeinDeviceInfo(pVeinAct);
        m_pModule->veinModuleActvalueList.append(pVeinAct); // auto delete / meta-data / scpi

        pVeinAct = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                            QString("ACT_DeviceState%1").arg(souceCount),
                                            QString("Component with source status"),
                                            QJsonObject());
        sourceVeinInterface->setVeinDeviceState(pVeinAct);
        m_pModule->veinModuleActvalueList.append(pVeinAct); // auto delete / meta-data / scpi

        // device param
        pVeinParam = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                            key = QString("PAR_SourceState%1").arg(souceCount),
                                                            QString("Component all source parameters in JSON format"),
                                                            QJsonObject());
        sourceVeinInterface->setVeinDeviceParameter(pVeinParam);
        //pVeinParam->setSCPIInfo(new cSCPIInfo("CONFIGURATION","RANGE", "10", "PAR_NominalRange", "0", s));
        jsonValidator = new cJsonParamValidator();
        sourceVeinInterface->setVeinDeviceParameterValidator(jsonValidator);
        pVeinParam->setValidator(jsonValidator);
        m_pModule->veinModuleParameterHash[key] = pVeinParam; // auto delete / meta-data / scpi

        m_arrVeinSourceInterfaces.append(sourceVeinInterface);
    }
}

QVariant cSourceModuleProgram::RPC_ScanInterface(QVariantMap p_params)
{
    int interfaceType = p_params["p_type"].toInt();
    QString deviceInfo = p_params["p_deviceInfo"].toString();
    QUuid veinUuid = p_params[VeinComponent::RemoteProcedureData::s_callIdString].toUuid();
    m_pSourceDeviceManager->startSourceScan(SourceInterfaceType(interfaceType), deviceInfo, veinUuid);
    return true;
}


configuration *cSourceModuleProgram::getConfigXMLWrapper()
{
    return static_cast<cSourceModuleConfiguration*>(m_pConfiguration.get())->getConfigXMLWrapper();
}

void cSourceModuleProgram::updateDemoCount()
{
    m_bDeafenDemoChange = true;
    m_pVeinDemoSourceCount->setValue(m_pSourceDeviceManager->demoCount());
    m_bDeafenDemoChange = false;
}

void cSourceModuleProgram::onSourceScanFinished(int slotPosition, cSourceDevice* device, QUuid uuid, QString errMsg)
{
    bool sourceAdded = slotPosition >= 0 && device;
    if(sourceAdded) {
        device->setVeinInterface(m_arrVeinSourceInterfaces[slotPosition]);
        m_pVeinCountAct->setValue(QVariant(m_pSourceDeviceManager->activeSlotCount()));
        updateDemoCount();
    }
    m_sharedPtrRpcScanInterface->sendRpcResult(uuid,
                                               sourceAdded ? VfCpp::cVeinModuleRpc::RPCResultCodes::RPC_SUCCESS : VfCpp::cVeinModuleRpc::RPCResultCodes::RPC_EINVAL,
                                               errMsg,
                                               sourceAdded);
}

void cSourceModuleProgram::onSourceDeviceRemoved(int slotPosition)
{
    Q_UNUSED(slotPosition)
    m_pVeinCountAct->setValue(QVariant(m_pSourceDeviceManager->activeSlotCount()));
    updateDemoCount();
}


void cSourceModuleProgram::newDemoSourceCount(QVariant demoCount)
{
    if(!m_bDeafenDemoChange) {
        m_pSourceDeviceManager->setDemoCount(demoCount.toInt());
    }
}

}
