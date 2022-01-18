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

    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open(info);

    SourceDevice sourceDevice(interface, type, name, version);
    QCOMPARE(type, sourceDevice.getType());
    QCOMPARE(name, sourceDevice.getName());
    QCOMPARE(version, sourceDevice.getVersion());
    QCOMPARE(info, sourceDevice.getInterfaceInfo());
}

void SourceDeviceTest::nonDemoInterFaceGet()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_BASE);
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    QVERIFY(!sourceDevice.isDemo());
}

void SourceDeviceTest::demoInterFaceGet()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    QVERIFY(sourceDevice.isDemo());
}

void SourceDeviceTest::disconnectSignal()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");

    int countDisconnectReceived = 0;
    connect(&sourceDevice, &SourceDevice::sigInterfaceDisconnected, [&] {
        countDisconnectReceived++;
    });
    sourceDevice.simulateExternalDisconnect();
    QTest::qWait(10);
    QCOMPARE(countDisconnectReceived, 1);
}

static IoWorkerCmdPack createWorkingCmdPack(SourceDevice *source)
{
    IoPacketGenerator ioPackGenerator = source->getIoPacketGenerator();
    JsonParamApi params;
    params.setOn(false);
    IoCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    return IoWorkerConverter::commandPackToWorkerPack(cmdPack);
}

void SourceDeviceTest::multipleCmdsDifferentIds()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");

    int id1 = sourceDevice.startTransaction(createWorkingCmdPack(&sourceDevice));
    int id2 = sourceDevice.startTransaction(createWorkingCmdPack(&sourceDevice));
    QVERIFY(id1 != id2);
}

class TestObserver : public SourceDeviceObserver {
public:
    int observerReceiveCount = 0;
    int observerReceiveId = 0;
    TestObserver(SourceDeviceSubject* subject) : SourceDeviceObserver(subject) {}
protected:
    virtual void updateResponse(IoWorkerCmdPack cmdPack) override {
        observerReceiveCount++;
        observerReceiveId = cmdPack.m_workerId;
    }
};

void SourceDeviceTest::observerReceiveCount()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");

    TestObserver testObserver1(&sourceDevice);
    TestObserver testObserver2(&sourceDevice);

    sourceDevice.startTransaction(createWorkingCmdPack(&sourceDevice));
    sourceDevice.startTransaction(createWorkingCmdPack(&sourceDevice));
    QTest::qWait(10); // source's worker requires event loop
    QCOMPARE(testObserver1.observerReceiveCount, 2);
    QCOMPARE(testObserver2.observerReceiveCount, 2);
}

void SourceDeviceTest::observerReceiveId()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");

    TestObserver testObserver1(&sourceDevice);
    TestObserver testObserver2(&sourceDevice);

    int id1 = sourceDevice.startTransaction(createWorkingCmdPack(&sourceDevice));
    QTest::qWait(10);
    QCOMPARE(testObserver1.observerReceiveId, id1);
    QCOMPARE(testObserver2.observerReceiveId, id1);

    int id2 = sourceDevice.startTransaction(createWorkingCmdPack(&sourceDevice));
    QTest::qWait(10);
    QCOMPARE(testObserver1.observerReceiveId, id2);
    QCOMPARE(testObserver2.observerReceiveId, id2);
}

void SourceDeviceTest::busySignalOnSwitch()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 1);

    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(SOURCE_MT_COMMON, "", "");
    IoPacketGenerator ioPackGen = IoPacketGenerator(paramStructure);

    ZeraJsonParamsState jsonParamsState(paramStructure);
    jsonParamsState.setStructure(paramStructure);
    JsonParamApi paramApi;
    paramApi.setParams(jsonParamsState.getDefaultJsonState());
    IoCommandPacket cmdPack = ioPackGen.generateOnOffPacket(paramApi);
    IoWorkerCmdPack workerPack = IoWorkerConverter::commandPackToWorkerPack(cmdPack);

    int countSwitches = 2;
    sourceDevice.startTransaction(workerPack);
    sourceDevice.startTransaction(workerPack);

    int busyToggleCount = 0;
    QObject::connect(&sourceDevice, &SourceDevice::sigSwitchTransationStarted, [&] {
        busyToggleCount++;
    });

    QTest::qWait(10);
    QCOMPARE(busyToggleCount, countSwitches);
}
