#ifndef SOURCEDEVICEINTERNAL_H
#define SOURCEDEVICEINTERNAL_H

#include "sourcedevicetemplate.h"
#include <pcbinterface.h>

class SourceDeviceInternal : public SourceDeviceTemplate
{
    Q_OBJECT
public:
    explicit SourceDeviceInternal(AbstractServerInterfacePtr serverInterface,
                                  const QJsonObject &sourceCapabilities);

    void setStatusPollTime(int ms) override;
    bool close(QUuid uuid) override;
};

#endif // SOURCEDEVICEINTERNAL_H
