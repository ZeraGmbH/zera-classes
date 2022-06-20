#include "main-unittest-qt.h"
#include "io-device/iodevicefactory.h"

static QList<QObject*> listTests;
static QList<Ttest> listTestFuncs;

QObject *addTest(QObject *test)
{
    listTests.append(test);
    return test;
}

Ttest addTestFunc(Ttest ptr)
{
    listTestFuncs.append(ptr);
    return ptr;
}

int main(int argc, char *argv[])
{
    int status = 0;
    Q_INIT_RESOURCE(resource);

    // Make qCritical in tests for re-used IoTransferDataSingle objects - see
    // IoDeviceBase::prepareSendAndReceive
    qputenv("QT_FATAL_CRITICALS", "1");

    while (!listTestFuncs.isEmpty()) {
        status |= listTestFuncs.takeLast()(argc, argv);
    }
    while(!listTests.isEmpty()) {
        QCoreApplication app(argc, argv);
        QObject *test = listTests.takeLast();
        status |= QTest::qExec(test, argc, argv);
        delete test;
    }
    if(status) {
#ifdef QT_DEBUG
        qWarning("\n\033[1;31m%s\033[0m","One or more tests failed!!!");
#else
        qWarning("\nOne or more tests failed!!!");
#endif
    }
    else {
#ifdef QT_DEBUG
        qInfo("\n\033[1;32m%s\033[0m","All tests passed :)");
#else
        qInfo("\nAll tests passed :)");
#endif
    }
    return status;
}

IoDeviceBase::Ptr createOpenDemoIoDevice(QString deviceInfo)
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    ioDevice->open(deviceInfo);
    return ioDevice;
}

void setDemoIoFixedTimeout(IoDeviceBase::Ptr ioDevice, int timeoutMs)
{
    IoDeviceDemo *demoDev = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoDev->setResponseDelay(false, timeoutMs);
}

