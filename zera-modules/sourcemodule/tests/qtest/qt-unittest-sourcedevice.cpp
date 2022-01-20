#include "main-unittest-qt.h"
#include "qt-unittest-sourcedevice.h"
#include "source-device/sourcedevice.h"
#include "source-device/sourcedeviceobserver.h"
#include "json/jsonstructureloader.h"
#include <zera-json-params-state.h>

static QObject* pSourceDeviceTest = addTest(new SourceDeviceTest);

void SourceDeviceTest::init()
{
}

void SourceDeviceTest::gettersOK()
{
    QString name = "fooName";
    QString version = "fooVersion";
    SupportedSourceTypes type = SOURCE_MT_COMMON;
    QString info = "fooInfo";

    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    interface->open(info);

    SourceDevice sourceDevice(interface, type, name, version);
    QCOMPARE(type, sourceDevice.getType());
    QCOMPARE(name, sourceDevice.getName());
    QCOMPARE(version, sourceDevice.getVersion());
    QCOMPARE(info, sourceDevice.getInterfaceInfo());
}

void SourceDeviceTest::nonDemoInterFaceGet()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    QVERIFY(!sourceDevice.isDemo());
}

void SourceDeviceTest::demoInterFaceGet()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    QVERIFY(sourceDevice.isDemo());
}

void SourceDeviceTest::disconnectSignal()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");

    int countDisconnectReceived = 0;
    connect(&sourceDevice, &SourceDevice::sigInterfaceDisconnected, [&] {
        countDisconnectReceived++;
    });
    sourceDevice.simulateExternalDisconnect();
    QTest::qWait(10);
    QCOMPARE(countDisconnectReceived, 1);
}

static IoTransferDataGroup createWorkingIoGroup(SourceDevice *source)
{
    IoGroupGenerator ioGroupGenerator = source->getIoGroupGenerator();
    JsonParamApi params;
    params.setOn(false);
    return ioGroupGenerator.generateOnOffGroup(params);
}

void SourceDeviceTest::multipleCmdsDifferentIds()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");

    int id1 = sourceDevice.startTransaction(createWorkingIoGroup(&sourceDevice));
    int id2 = sourceDevice.startTransaction(createWorkingIoGroup(&sourceDevice));
    QVERIFY(id1 != id2);
}

class TestObserver : public SourceDeviceObserver {
public:
    int observerReceiveCount = 0;
    int observerReceiveId = 0;
    TestObserver(SourceDeviceSubject* subject) : SourceDeviceObserver(subject) {}
protected:
    virtual void updateResponse(IoTransferDataGroup transferGroup) override {
        observerReceiveCount++;
        observerReceiveId = transferGroup.getGroupId();
    }
};

void SourceDeviceTest::observerReceiveCount()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");

    TestObserver testObserver1(&sourceDevice);
    TestObserver testObserver2(&sourceDevice);

    sourceDevice.startTransaction(createWorkingIoGroup(&sourceDevice));
    sourceDevice.startTransaction(createWorkingIoGroup(&sourceDevice));
    QTest::qWait(10); // source's queue requires event loop
    QCOMPARE(testObserver1.observerReceiveCount, 2);
    QCOMPARE(testObserver2.observerReceiveCount, 2);
}

void SourceDeviceTest::observerReceiveId()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");

    TestObserver testObserver1(&sourceDevice);
    TestObserver testObserver2(&sourceDevice);

    int id1 = sourceDevice.startTransaction(createWorkingIoGroup(&sourceDevice));
    QTest::qWait(10);
    QCOMPARE(testObserver1.observerReceiveId, id1);
    QCOMPARE(testObserver2.observerReceiveId, id1);

    int id2 = sourceDevice.startTransaction(createWorkingIoGroup(&sourceDevice));
    QTest::qWait(10);
    QCOMPARE(testObserver1.observerReceiveId, id2);
    QCOMPARE(testObserver2.observerReceiveId, id2);
}

void SourceDeviceTest::busySignalOnSwitch()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 1);

    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(SOURCE_MT_COMMON, "", "");
    IoGroupGenerator ioGroupGen = IoGroupGenerator(paramStructure);

    ZeraJsonParamsState jsonParamsState(paramStructure);
    jsonParamsState.setStructure(paramStructure);
    JsonParamApi paramApi;
    paramApi.setParams(jsonParamsState.getDefaultJsonState());
    IoTransferDataGroup transferGroup = ioGroupGen.generateOnOffGroup(paramApi);

    int countSwitches = 2;
    sourceDevice.startTransaction(transferGroup);
    sourceDevice.startTransaction(transferGroup);

    int busyToggleCount = 0;
    QObject::connect(&sourceDevice, &SourceDevice::sigSwitchTransationStarted, [&] {
        busyToggleCount++;
    });

    QTest::qWait(10);
    QCOMPARE(busyToggleCount, countSwitches);
}
