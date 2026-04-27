#ifndef SOURCEDEVICEINTERNAL_H
#define SOURCEDEVICEINTERNAL_H

#include "sourcedevicetemplate.h"
#include "sourcestatecontrollerinternal.h"
#include <pcbinterface.h>

class SourceDeviceInternal : public SourceDeviceTemplate
{
    Q_OBJECT
public:
    explicit SourceDeviceInternal(AbstractServerInterfacePtr serverInterface,
                                  const QJsonObject &sourceCapabilities);

    bool close(QUuid uuid) override;
private slots:
    void onSourceSwitchFinished(bool ok);
private:
    SourceStateControllerInternal m_stateController;
};

#endif // SOURCEDEVICEINTERNAL_H
