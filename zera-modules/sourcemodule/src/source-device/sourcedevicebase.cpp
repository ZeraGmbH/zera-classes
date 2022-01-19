#include "sourcedevicebase.h"
#include "json/jsonstructureloader.h"
#include "io-device/iodevicedemo.h"

SourceDeviceBase::SourceDeviceBase(tIoDeviceShPtr interface, SupportedSourceTypes type, QString deviceName, QString version) :
    QObject(nullptr),
    m_ioInterface(interface)
{
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(type, deviceName, version);
    m_ioGroupGenerator = new IoGroupGenerator(paramStructure);

    connect(&m_ioWorker, &IoWorker::sigTransferGroupFinished,
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
    return m_ioInterface->isDemo();
}

QString SourceDeviceBase::getInterfaceDeviceInfo()
{
    return m_ioInterface->getDeviceInfo();
}

void SourceDeviceBase::switchState(JsonParamApi state)
{
    m_paramsRequested = state;
    IoTransferDataGroup transferGroup = m_ioGroupGenerator->generateOnOffGroup(m_paramsRequested);
    if(isDemo()) {
        IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(m_ioInterface.get());
        demoInterface->setResponseDelay(m_bDemoDelayFollowsTimeout, 0);
    }
    m_currWorkerId.setCurrent(m_ioWorker.enqueueTransferGroup(transferGroup));
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
    if(m_currWorkerId.isCurrAndDeactivateIf(transferGroup.m_groupId)) {
        handleSourceCmd(transferGroup);
    }
}
