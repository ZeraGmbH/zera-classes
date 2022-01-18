#include "sourcedevicestatuswatcherjson.h"

SourceDeviceStatusWatcherJson::SourceDeviceStatusWatcherJson(SourceDevice *sourceDevice) :
    SourceDeviceObserver(sourceDevice),
    m_sourceDevice(sourceDevice),
    m_stateEnum(SOURCE_STATE::UNDEFINED)
{
    m_pollTimer.setSingleShot(false);
    m_pollTimer.setInterval(500);
    enablePolling();
    connect(&m_pollTimer, &QTimer::timeout,
            this, &SourceDeviceStatusWatcherJson::onPollTimer);
    connect(m_sourceDevice, &SourceDevice::sigSwitchTransationStarted,
            this, &SourceDeviceStatusWatcherJson::onSwitchBusyChanged);
}

void SourceDeviceStatusWatcherJson::enablePolling()
{
    m_pollTimer.start();
}

void SourceDeviceStatusWatcherJson::disablePolling()
{
    m_pollTimer.stop();
}

void SourceDeviceStatusWatcherJson::setPollTime(int ms)
{
    bool active = m_pollTimer.isActive();
    if(active) {
        disablePolling();
    }
    m_pollTimer.setInterval(ms);
    if(active) {
        enablePolling();
    }
}

bool SourceDeviceStatusWatcherJson::isErrorActive()
{
    return m_stateEnum == SOURCE_STATE::ERROR_SWITCH ||
            m_stateEnum == SOURCE_STATE::ERROR_POLL;
}

void SourceDeviceStatusWatcherJson::onSwitchBusyChanged()
{
    // switch is the only way out of error -> set busy unconditional
    setState(SOURCE_STATE::SWITCH_BUSY);
}

void SourceDeviceStatusWatcherJson::updateResponse(IoWorkerCmdPack cmdPack)
{
    if(cmdPack.isSwitchPack()) {
        handleSwitchResponse(cmdPack);
    }

    if(cmdPack.isStateQuery() && m_ioIdKeeper.isCurrAndDeactivateIf(cmdPack.m_workerId)) {
        handleStateResponse(cmdPack);
    }
}

void SourceDeviceStatusWatcherJson::onPollTimer()
{
    if(!m_ioIdKeeper.isActive()) {
        IoCommandPacket cmdPack = m_sourceDevice->getIoPacketGenerator().generateStatusPollPacket();
        IoWorkerCmdPack workerPack = IoWorkerConverter::commandPackToWorkerPack(cmdPack);
        m_ioIdKeeper.setCurrent(m_sourceDevice->startTransaction(workerPack));
    }
}

void SourceDeviceStatusWatcherJson::setState(SOURCE_STATE state)
{
    if(state != m_stateEnum) {
        m_stateEnum = state;
        emit sigStateChanged(m_stateEnum);
        setPollingOnStateChange();
    }
}

void SourceDeviceStatusWatcherJson::setPollingOnStateChange()
{
    if(!isErrorActive()) {
        enablePolling();
    }
    else {
        disablePolling();
    }
}

void SourceDeviceStatusWatcherJson::handleSwitchResponse(IoWorkerCmdPack &cmdPack)
{
    if(!cmdPack.passedAll() && !isErrorActive()) {
        setState(SOURCE_STATE::ERROR_SWITCH);
    }
    else if(m_stateEnum == SOURCE_STATE::SWITCH_BUSY) {
        setState(SOURCE_STATE::IDLE);
    }
}

void SourceDeviceStatusWatcherJson::handleStateResponse(IoWorkerCmdPack &cmdPack)
{
    if(!cmdPack.passedAll() && !isErrorActive()) {
        setState(SOURCE_STATE::ERROR_POLL);
    }
    else if(m_stateEnum == SOURCE_STATE::UNDEFINED) {
        setState(SOURCE_STATE::IDLE);
    }
}
