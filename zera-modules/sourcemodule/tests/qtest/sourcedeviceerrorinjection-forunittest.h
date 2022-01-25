#ifndef SOURCEDEVICEFORUNITTEST_H
#define SOURCEDEVICEFORUNITTEST_H

#include "source-device/sourcedevice.h"

class SourceDeviceErrorInjection : public SourceDeviceInterface
{
public:
    SourceDeviceErrorInjection(SourceDeviceInterface *sourceUnderTest);

    int startTransaction(IoTransferDataGroup::Ptr transferGroup) override;
    IoGroupGenerator getIoGroupGenerator() const override;
    SourceProperties getProperties() const override;

    void setDemoResonseError(bool active);

private:
    bool m_demoSimulErrorActive = false;
    SourceDeviceInterface *m_sourceUnderTest = nullptr;
};

#endif // SOURCEDEVICEFORUNITTEST_H
