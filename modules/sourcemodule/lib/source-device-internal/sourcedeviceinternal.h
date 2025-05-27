#ifndef SOURCEDEVICEINTERNAL_H
#define SOURCEDEVICEINTERNAL_H

#include <sourcedevicetemplate.h>

class SourceDeviceInternal : public SourceDeviceTemplate
{
    Q_OBJECT
public:
    explicit SourceDeviceInternal(IoDeviceBase::Ptr ioDevice, const QJsonObject &sourceJsonStruct);

    void setStatusPollTime(int ms) override;
    bool close(QUuid uuid) override;
};

#endif // SOURCEDEVICEINTERNAL_H
