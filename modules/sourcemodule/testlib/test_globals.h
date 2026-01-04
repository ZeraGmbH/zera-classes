#ifndef TEST_GLOBALS_H
#define TEST_GLOBALS_H

#include <QtTest>
#include "iodevicebase.h"
#include "sourceproperties.h"

static constexpr int shortQtEventTimeout = 15;

IoDeviceBase::Ptr createOpenDemoIoDevice(QString deviceInfo = QString());
void setDemoIoFixedTimeout(IoDeviceBase::Ptr ioDevice, int timeoutMs);


class DefaultTestSourceProperties : public SourceProperties
{
public:
    DefaultTestSourceProperties() :
        SourceProperties(SOURCE_MT_COMMON, "", "", SourceProtocols::ZERA_SERIAL, SourceProperties::EXTERNAL)
    {}
};

#endif // TEST_GLOBALS_H
