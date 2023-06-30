#ifndef TEST_GLOBALS_H
#define TEST_GLOBALS_H

#include <QtTest>
#include "iodevicebase.h"
#include "iodevicedemo.h"
#include "sourceproperties.h"

#undef QTEST_MAIN
#define QTEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    Q_INIT_RESOURCE(resource); \
    /* Make qCritical in tests for re-used IoTransferDataSingle objects - see IoDeviceBase::prepareSendAndReceive */ \
    qputenv("QT_FATAL_CRITICALS", "1"); \
    QTEST_MAIN_IMPL(TestObject) \
}


static constexpr int shortQtEventTimeout = 15;

IoDeviceBase::Ptr createOpenDemoIoDevice(QString deviceInfo = QString());
void setDemoIoFixedTimeout(IoDeviceBase::Ptr ioDevice, int timeoutMs);


class DefaultTestSourceProperties : public SourceProperties
{
public:
    DefaultTestSourceProperties() :
        SourceProperties(SOURCE_MT_COMMON, "", "", SourceProtocols::ZERA_SERIAL)
    {}
};

#endif // TEST_GLOBALS_H
