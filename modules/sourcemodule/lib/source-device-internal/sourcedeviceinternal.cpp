#include "sourcedeviceinternal.h"
#include "sourceswitchjsoninternal.h"

SourceDeviceInternal::SourceDeviceInternal(AbstractServerInterfacePtr serverInterface, const QJsonObject &sourceCapabilities) :
    SourceDeviceTemplate(JsonStructApi(sourceCapabilities).getDeviceName(), IoDeviceTypes::SCPI_NET, sourceCapabilities)
{
    m_switcher = std::make_unique<SourceSwitchJsonInternal>(serverInterface, sourceCapabilities);
    connect(&m_stateController, &SourceStateControllerInternal::sigStateChanged,
            this, &SourceDeviceInternal::handleNewState);
}

bool SourceDeviceInternal::close(QUuid uuid)
{
    resetVeinComponents();
    emit sigClosed(getId(), uuid);
    return true;
}
