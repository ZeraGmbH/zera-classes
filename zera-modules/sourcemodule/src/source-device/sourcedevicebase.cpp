#include "sourcedevicebase.h"
#include "json/jsonstructureloader.h"
#include "io-device/iodevicedemo.h"

SourceDeviceBase::SourceDeviceBase(IoDeviceBase::Ptr ioDevice, SupportedSourceTypes type, QString deviceName, QString version) :
    QObject(nullptr),
    m_ioDevice(ioDevice)
{
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(type, deviceName, version);
    m_ioGroupGenerator = new IoGroupGenerator(paramStructure);

    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceDeviceBase::onIoGroupFinished);
}

SourceDeviceBase::~SourceDeviceBase()
{
    delete m_ioGroupGenerator;
}

void SourceDeviceBase::setDemoDelayFollowsTimeout(bool demoDelayFollowsTimeout)
{
    m_bDemoDelayFollowsTimeout = demoDelayFollowsTimeout;
}

bool SourceDeviceBase::isDemo()
{
    return m_ioDevice->isDemo();
}

QString SourceDeviceBase::getInterfaceDeviceInfo()
{
    return m_ioDevice->getDeviceInfo();
}

void SourceDeviceBase::switchState(JsonParamApi state)
{
    m_paramsRequested = state;
    IoTransferDataGroup transferGroup = m_ioGroupGenerator->generateOnOffGroup(m_paramsRequested);
    if(isDemo()) {
        IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(m_ioDevice.get());
        demoIoDevice->setResponseDelay(m_bDemoDelayFollowsTimeout, 0);
    }
    m_currQueueId.setCurrent(m_ioQueue.enqueueTransferGroup(transferGroup));
}

void SourceDeviceBase::switchOff()
{
    m_paramsCurrent.setOn(false);
    switchState(m_paramsCurrent);
}

void SourceDeviceBase::handleSourceCmd(IoTransferDataGroup transferGroup)
{
    if(transferGroup.passedAll()) {
        m_paramsCurrent = m_paramsRequested;
    }
}

void SourceDeviceBase::onIoGroupFinished(IoTransferDataGroup transferGroup)
{
    if(m_currQueueId.isCurrAndDeactivateIf(transferGroup.getGroupId())) {
        handleSourceCmd(transferGroup);
    }
}
