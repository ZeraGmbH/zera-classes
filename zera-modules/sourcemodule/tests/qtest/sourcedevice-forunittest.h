#ifndef SOURCEDEVICEFORUNITTEST_H
#define SOURCEDEVICEFORUNITTEST_H

#include "source-device/sourcedevice.h"

class SourceDeviceForUnittest : public SourceDevice
{
public:
    SourceDeviceForUnittest(tIoDeviceShPtr interface, SupportedSourceTypes type, QString name, QString version);
    void setDemoResonseError(bool active);
    virtual int startTransaction(IoTransferDataGroup transferGroup) override;
private:
    bool m_demoSimulErrorActive = false;
};

#endif // SOURCEDEVICEFORUNITTEST_H
