#include "sourcedeviceinternal.h"
#include "sourceswitchjsoninternal.h"

SourceDeviceInternal::SourceDeviceInternal(AbstractServerInterfacePtr serverInterface, const QJsonObject &sourceCapabilities) :
    SourceDeviceTemplate(JsonStructApi(sourceCapabilities).getDeviceName(), IoDeviceTypes::SCPI_NET, sourceCapabilities)
{
    m_switcher = std::make_unique<SourceSwitchJsonInternal>(serverInterface, sourceCapabilities);
    connect(m_switcher.get(), &AbstractSourceSwitchJson::sigSwitchFinished,
            this, &SourceDeviceInternal::onSourceSwitchFinished);
}

void SourceDeviceInternal::switchLoad(const QJsonObject &params)
{
    JsonParamApi paramApi;
    paramApi.setParams(params);
    m_lastOnState = paramApi.getOn();
    m_switcher->switchState(paramApi);
    handleNewState(SourceStates::SWITCH_BUSY);
}

bool SourceDeviceInternal::close(QUuid uuid)
{
    resetVeinComponents();
    emit sigClosed(getId(), uuid);
    return true;
}

void SourceDeviceInternal::onSourceSwitchFinished(bool ok)
{
    setVeinLoadpointParam(m_switcher->getCurrLoadpoint().getParams());
    handleNewState(ok ? SourceStates::IDLE : SourceStates::ERROR_SWITCH);
}
