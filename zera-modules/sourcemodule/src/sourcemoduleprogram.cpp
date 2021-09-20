#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <modulevalidator.h>
#include <intvalidator.h>

#include "basedspmeasprogram.h"
#include "sourcemoduleprogram.h"
#include "sourcemodule.h"
#include "sourcedevicemanager.h"
#include "sourcedevice.h"
#include "iointerface.h"
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
}


void cSourceModuleProgram::generateInterface()
{
    QString key;
    configuration* config = getConfigXMLWrapper();
    m_pSourceDeviceManager = new cSourceDeviceManager(config->max_count_sources());
    connect(m_pSourceDeviceManager, &cSourceDeviceManager::sigSlotAdded, this, &cSourceModuleProgram::onSourceDeviceAdded);
    connect(m_pSourceDeviceManager, &cSourceDeviceManager::sigSlotRemoved, this, &cSourceModuleProgram::onSourceDeviceRemoved);

    m_pVeinMaxCountAct = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                        QString("ACT_MaxSources"),
                                        QString("Component with max source devices handled"),
                                        QVariant(config->max_count_sources()) );
    m_pModule->veinModuleActvalueList.append(m_pVeinMaxCountAct); // auto delete / meta-data / scpi

    m_pVeinDemoSourceCount = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                           key = QString("PAR_DemoSources"),
                                                           QString("Component for setting number of demo sources"),
                                                           QVariant(int(0)));
    m_pVeinDemoSourceCount->setValidator(new cIntValidator(0,4));
    connect(m_pVeinDemoSourceCount, &cVeinModuleParameter::sigValueChanged, this, &cSourceModuleProgram::newDemoSourceCount);
    m_pModule->veinModuleParameterHash[key] = m_pVeinDemoSourceCount; // auto delete / meta-data / scpi
}


configuration *cSourceModuleProgram::getConfigXMLWrapper()
{
    return static_cast<cSourceModuleConfiguration*>(m_pConfiguration.get())->getConfigXMLWrapper();
}


void cSourceModuleProgram::onSourceDeviceAdded(int slotPosition)
{

}

void cSourceModuleProgram::onSourceDeviceRemoved(int slotPosition)
{

}


static bool randomBool() {
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
            cIOInterface* demoInterface = new cIOInterface(cIOInterface::IO_DEMO, randomString(16));
            m_pSourceDeviceManager->startSourceIdentification(demoInterface);
            sourcesToAdd--;
        }
    }
    else {
        int sourcesToRemove = activeSlotCount - iDemoCount;
        // https://stackoverflow.com/questions/43329352/generating-random-boolean/43329456
        static auto genRandomBool = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
        bool removeFront = randomBool();
        if(removeFront) {
            for(int slotNo=0; sourcesToRemove && slotNo<getConfigXMLWrapper()->max_count_sources(); slotNo++) {
                cSourceDevice* source = m_pSourceDeviceManager->getSourceDevice(slotNo);
                if(source) {
                    source->ioInterface()->requestExternalDisconnect();
                    sourcesToRemove--;
                }
            }
        }
        else {
            for(int slotNo=getConfigXMLWrapper()->max_count_sources()-1; sourcesToRemove && slotNo>=0; slotNo--) {
                cSourceDevice* source = m_pSourceDeviceManager->getSourceDevice(slotNo);
                if(source) {
                    source->ioInterface()->requestExternalDisconnect();
                    sourcesToRemove--;
                }
            }
        }
    }
}

}
