#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <modulevalidator.h>
#include <intvalidator.h>
#include <jsonparamvalidator.h>

#include "basedspmeasprogram.h"
#include "sourcemoduleprogram.h"
#include "sourcemodule.h"
#include "sourcedevicemanager.h"
#include "sourcedevice.h"
#include "sourceveininterface.h"
#include "sourceinterface.h"
#include <random>

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
    QString key;
    configuration* config = getConfigXMLWrapper();
    int maxSources = config->max_count_sources();

    // common components
    m_pSourceDeviceManager = new cSourceDeviceManager(maxSources);
    connect(m_pSourceDeviceManager, &cSourceDeviceManager::sigSlotAdded, this, &cSourceModuleProgram::onSourceDeviceAdded);
    connect(m_pSourceDeviceManager, &cSourceDeviceManager::sigSlotRemoved, this, &cSourceModuleProgram::onSourceDeviceRemoved);

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


configuration *cSourceModuleProgram::getConfigXMLWrapper()
{
    return static_cast<cSourceModuleConfiguration*>(m_pConfiguration.get())->getConfigXMLWrapper();
}

void cSourceModuleProgram::onSourceDeviceAdded(int slotPosition, cSourceDevice* device)
{
    device->setVeinInterface(m_arrVeinSourceInterfaces[slotPosition]);
    m_pVeinCountAct->setValue(QVariant(m_pSourceDeviceManager->activeSlotCount()));
}

void cSourceModuleProgram::onSourceDeviceRemoved(int slotPosition)
{
    Q_UNUSED(slotPosition)
    m_pVeinCountAct->setValue(QVariant(m_pSourceDeviceManager->activeSlotCount()));
}


static bool randomBool() {
    // https://stackoverflow.com/questions/43329352/generating-random-boolean/43329456
    static auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    return gen();
}

QString randomString(int length) {
    QString strRnd;
    for(int cchar=0; cchar<length;cchar++) {
        strRnd += QString("%1").arg(randomBool());
    }
    return strRnd;
}

void cSourceModuleProgram::newDemoSourceCount(QVariant demoCount)
{
    int activeSlotCount = m_pSourceDeviceManager->activeSlotCount();
    int iDemoCount = demoCount.toInt();
    if(iDemoCount > activeSlotCount) {
        int sourcesToAdd = iDemoCount - activeSlotCount;
        while(sourcesToAdd) {
            cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
            m_pSourceDeviceManager->startSourceIdentification(interface);
            sourcesToAdd--;
        }
    }
    else {
        int sourcesToRemove = activeSlotCount - iDemoCount;
        bool removeFront = randomBool();
        if(removeFront) {
            for(int slotNo=0; sourcesToRemove && slotNo<getConfigXMLWrapper()->max_count_sources(); slotNo++) {
                cSourceDevice* source = m_pSourceDeviceManager->sourceDevice(slotNo);
                if(source) {
                    static_cast<cSourceInterfaceDemo*>(source->ioInterface())->simulateExternalDisconnect();
                    sourcesToRemove--;
                }
            }
        }
        else {
            for(int slotNo=getConfigXMLWrapper()->max_count_sources()-1; sourcesToRemove && slotNo>=0; slotNo--) {
                cSourceDevice* source = m_pSourceDeviceManager->sourceDevice(slotNo);
                if(source) {
                    static_cast<cSourceInterfaceDemo*>(source->ioInterface())->simulateExternalDisconnect();
                    sourcesToRemove--;
                }
            }
        }
    }
}

}
