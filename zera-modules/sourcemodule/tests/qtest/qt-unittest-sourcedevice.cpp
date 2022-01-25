#include "main-unittest-qt.h"
#include "qt-unittest-sourcedevice.h"
#include "io-device/iodevicefactory.h"
#include "io-device/iotransferdatasinglefactory.h"
#include "source-device/sourcedevice.h"
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

    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    ioDevice->open(info);

    SourceProperties sourceProperties(SOURCE_MT_COMMON, name, version);
    SourceDevice sourceDevice(ioDevice, sourceProperties);

    QCOMPARE(type, sourceDevice.getProperties().getType());
    QCOMPARE(name, sourceDevice.getProperties().getName());
    QCOMPARE(version, sourceDevice.getProperties().getVersion());
}

void SourceDeviceTest::signalDisconnect()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceDevice sourceDevice(ioDevice, sourceProperties);

    int countDisconnectReceived = 0;
    connect(&sourceDevice, &SourceDevice::sigInterfaceDisconnected, [&] {
        countDisconnectReceived++;
    });
    sourceDevice.simulateExternalDisconnect();

    QTest::qWait(10);
    QCOMPARE(countDisconnectReceived, 1);
}

void SourceDeviceTest::signalResponses()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceDevice sourceDevice(ioDevice, sourceProperties);

    int countResponseReceived = 0;
    connect(&sourceDevice, &SourceDevice::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoTransferDataGroup::Ptr workTransferGroup1 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup1->appendTransferList(transList1);
    sourceDevice.startTransaction(workTransferGroup1);

    IoTransferDataGroup::Ptr workTransferGroup2 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup2->appendTransferList(transList2);
    sourceDevice.startTransaction(workTransferGroup2);

    QTest::qWait(10);
    QCOMPARE(countResponseReceived, 2);
}

void SourceDeviceTest::signalResponsesOnOneError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceDevice sourceDevice(ioDevice, sourceProperties);

    int countResponseReceived = 0;
    connect(&sourceDevice, &SourceDevice::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoTransferDataGroup::Ptr workTransferGroup1 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->setDemoErrorOnTransfer(0);
    sourceDevice.startTransaction(workTransferGroup1);

    IoTransferDataGroup::Ptr workTransferGroup2 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup2->appendTransferList(transList2);
    sourceDevice.startTransaction(workTransferGroup2);

    QTest::qWait(10);
    QCOMPARE(countResponseReceived, 2);
}

void SourceDeviceTest::signalResponsesOnTwoErrors()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceDevice sourceDevice(ioDevice, sourceProperties);

    int countResponseReceived = 0;
    connect(&sourceDevice, &SourceDevice::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoTransferDataGroup::Ptr workTransferGroup1 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->setDemoErrorOnTransfer(0);
    sourceDevice.startTransaction(workTransferGroup1);

    IoTransferDataGroup::Ptr workTransferGroup2 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup2->appendTransferList(transList2);
    workTransferGroup2->setDemoErrorOnTransfer(0);
    sourceDevice.startTransaction(workTransferGroup2);

    QTest::qWait(10);
    QCOMPARE(countResponseReceived, 2);
}
