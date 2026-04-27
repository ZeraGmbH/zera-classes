#include "sourcedeviceinternal.h"
#include "sourceswitchjsoninternal.h"

SourceDeviceInternal::SourceDeviceInternal(AbstractServerInterfacePtr serverInterface, const QJsonObject &sourceCapabilities) :
    SourceDeviceTemplate(JsonStructApi(sourceCapabilities).getDeviceName(), IoDeviceTypes::SCPI_NET, sourceCapabilities)
{
    // sigSwitchTransactionStarted is not part of abstract interface
    std::unique_ptr<SourceSwitchJsonInternal> switcher = std::make_unique<SourceSwitchJsonInternal>(serverInterface, sourceCapabilities);
    connect(switcher.get(), &SourceSwitchJsonInternal::sigSwitchTransactionStarted,
            this, &SourceDeviceInternal::onSwitchTransactionStarted);
    m_switcher = std::move(switcher);
    connect(m_switcher.get(), &AbstractSourceSwitchJson::sigSwitchFinished,
            this, &SourceDeviceInternal::onSourceSwitchFinished);
}

bool SourceDeviceInternal::close(QUuid uuid)
{
    resetVeinComponents();
    emit sigClosed(getId(), uuid);
    return true;
}

void SourceDeviceInternal::onSwitchTransactionStarted()
{
    handleNewState(SourceStates::SWITCH_BUSY);
}

void SourceDeviceInternal::onSourceSwitchFinished(bool ok)
{
    setVeinParamState(m_switcher->getCurrLoadState().getParams());
    handleNewState(ok ? SourceStates::IDLE : SourceStates::ERROR_SWITCH);
}
