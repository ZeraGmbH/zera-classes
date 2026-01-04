#include "test_sourcedevicefacade.h"
#include "iodevicedemo.h"
#include "test_globals.h"
#include "veincomponentsetnotifier-forunittest.h"
#include "sourcedeviceextserial.h"
#include "sourceveininterface.h"
#include "iodevicefactory.h"
#include "jsonstructureloader.h"
#include "persistentjsonstate.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"
#include <vfmoduleparameter.h>
#include <jsonparamvalidator.h>

#include <QString>

QTEST_MAIN(test_sourcedevicefacade)

void test_sourcedevicefacade::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_sourcedevicefacade::checkDeviceInfo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    QCOMPARE(ioDevice->getDeviceInfo(), sourceFacade.getIoDeviceInfo());
}

void test_sourcedevicefacade::checkDemoOn()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    QCOMPARE(sourceFacade.hasDemoIo(), true);
}

void test_sourcedevicefacade::checkDemoOff()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    QCOMPARE(sourceFacade.hasDemoIo(), false);
}

static const QString componentNameActState = QStringLiteral("act_state");
static const QString componentNameActInfo = QStringLiteral("act_info");
static const QString componentNameParLoad = QStringLiteral("par_load");
static const QString noDescription = QStringLiteral("no-description");
static constexpr int defaultEntityId = 1;

struct TVeinObjects
{
    SourceVeinInterface veinInterface;
    VfModuleComponent veinActDeviceState;
    VfModuleComponent veinActDeviceInfo;
    VfModuleParameter veinDeviceParameter;
    cJsonParamValidator veinDeviceParameterValidator;
    TVeinObjects(QJsonObject jsonStructure, VeinEvent::EventSystem *veinEventSystem) :
        veinActDeviceState(defaultEntityId,
                           veinEventSystem,
                           componentNameActState, noDescription, QVariant()),
        veinActDeviceInfo(defaultEntityId,
                          veinEventSystem,
                          componentNameActInfo, noDescription, QVariant()),
        veinDeviceParameter(defaultEntityId,
                            veinEventSystem,
                            componentNameParLoad, noDescription, QVariant())
    {
        veinInterface.setVeinDeviceStateComponent(&veinActDeviceState);
        veinInterface.setVeinDeviceInfoComponent(&veinActDeviceInfo);
        veinInterface.setVeinDeviceParameterComponent(&veinDeviceParameter);
        veinInterface.setVeinDeviceParameterValidator(&veinDeviceParameterValidator);
        veinDeviceParameterValidator.setJSonParameterStructure(jsonStructure);
    }
};

void test_sourcedevicefacade::checkVeinInitialStatus()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties());

    VeinComponentSetNotifier veinEventSystem(defaultEntityId);
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameActState, &vein.veinActDeviceState);
    QList<QJsonObject> statesReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameActState);
        QJsonObject jsonState = newValue.toJsonObject();
        statesReceived.append(jsonState);
    });

    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    sourceFacade.setVeinInterface(&vein.veinInterface);

    TimeMachineObject::feedEventLoop();
    QJsonObject jsonStatusVein = vein.veinActDeviceState.getValue().toJsonObject();
    JsonDeviceStatusApi statusApi;
    statusApi.setDeviceInfo(deviceInfo);
    QJsonObject jsonStatusDefault = statusApi.getJsonStatus();
    QCOMPARE(jsonStatusVein, jsonStatusDefault);
    QCOMPARE(statesReceived.count(), 1);
    QCOMPARE(statesReceived[0], jsonStatusDefault);
}

void test_sourcedevicefacade::checkVeinInitialInfo()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties());

    VeinComponentSetNotifier veinEventSystem(defaultEntityId);
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameActInfo, &vein.veinActDeviceInfo);
    QList<QJsonObject> infosReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameActInfo);
        QJsonObject jsonInfo = newValue.toJsonObject();
        infosReceived.append(jsonInfo);
    });

    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    sourceFacade.setVeinInterface(&vein.veinInterface);

    TimeMachineObject::feedEventLoop();
    QJsonObject jsonStructInfo = vein.veinActDeviceInfo.getValue().toJsonObject();
    QCOMPARE(jsonStructInfo, jsonStructure);
    QCOMPARE(infosReceived.count(), 1);
    QCOMPARE(infosReceived[0], jsonStructure);
}

void test_sourcedevicefacade::checkVeinInitialLoad()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties());

    VeinComponentSetNotifier veinEventSystem(defaultEntityId);
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameParLoad, &vein.veinDeviceParameter);
    QList<QJsonObject> loadsReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameParLoad);
        QJsonObject jsonInfo = newValue.toJsonObject();
        loadsReceived.append(jsonInfo);
    });

    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    sourceFacade.setVeinInterface(&vein.veinInterface);

    TimeMachineObject::feedEventLoop();
    QJsonObject jsonLoadParams = vein.veinDeviceParameter.getValue().toJsonObject();
    PersistentJsonState persistentState(JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties()));
    JsonParamApi paramApi = persistentState.loadJsonState();
    QJsonObject jsonLoadParamsDefault = paramApi.getParams();
    QCOMPARE(jsonLoadParams, jsonLoadParamsDefault);
    QCOMPARE(loadsReceived.count(), 1);
    QCOMPARE(loadsReceived[0], jsonLoadParamsDefault);
}

void test_sourcedevicefacade::checkVeinSwitchTwoStateChanges()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties());

    VeinComponentSetNotifier veinEventSystem(defaultEntityId);
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameActState, &vein.veinActDeviceState);
    QList<QJsonObject> statesReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameActState);
        QJsonObject jsonState = newValue.toJsonObject();
        statesReceived.append(jsonState);
    });

    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    sourceFacade.setStatusPollTime(0);
    sourceFacade.setVeinInterface(&vein.veinInterface);

    TimeMachineObject::feedEventLoop(); // initial vein processing
    JsonParamApi paramApi;
    paramApi.setParams(vein.veinDeviceParameter.getValue().toJsonObject());
    paramApi.setOn(true);
    sourceFacade.switchLoad(paramApi.getParams());

    TimeMachineForTest::getInstance()->processTimers(30);
    QCOMPARE(statesReceived.count(), 3);
    QCOMPARE(statesReceived[0]["busy"], false);
    QCOMPARE(statesReceived[1]["busy"], true);
    QCOMPARE(statesReceived[2]["busy"], false);
}

void test_sourcedevicefacade::checkVeinSwitchChangesLoad()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties());

    VeinComponentSetNotifier veinEventSystem(defaultEntityId);
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameParLoad, &vein.veinDeviceParameter);
    QList<QJsonObject> loadsReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameParLoad);
        QJsonObject jsonInfo = newValue.toJsonObject();
        loadsReceived.append(jsonInfo);
    });

    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    sourceFacade.setVeinInterface(&vein.veinInterface);

    TimeMachineObject::feedEventLoop(); // initial vein processing
    JsonParamApi paramApi;
    paramApi.setParams(vein.veinDeviceParameter.getValue().toJsonObject());
    paramApi.setOn(true);
    QJsonObject jsonLoadParamsOn = paramApi.getParams();
    sourceFacade.switchLoad(jsonLoadParamsOn);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    PersistentJsonState persistentState(JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties()));
    JsonParamApi paramApiDefault = persistentState.loadJsonState();
    QJsonObject jsonLoadParamsDefault = paramApiDefault.getParams();

    QCOMPARE(loadsReceived.count(), 2);
    QCOMPARE(loadsReceived[0], jsonLoadParamsDefault);
    QCOMPARE(loadsReceived[1], jsonLoadParamsOn);
}

void test_sourcedevicefacade::checkVeinSwitchError()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    IoDeviceDemo* demoIO = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIO->setAllTransfersError(true);
    demoIO->setResponseDelay(false, 1); // at least our vein stub gets confused on high fire
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties());

    VeinComponentSetNotifier veinEventSystem(defaultEntityId);
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameParLoad, &vein.veinDeviceParameter);
    veinEventSystem.addComponentToNotify(componentNameActState, &vein.veinActDeviceState);
    QList<QJsonObject> loadsReceived;
    QList<QJsonObject> statesReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QJsonObject json = newValue.toJsonObject();
        if(componentName == componentNameParLoad) {
            loadsReceived.append(json);
        }
        if(componentName == componentNameActState) {
            statesReceived.append(json);
        }
    });

    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    sourceFacade.setVeinInterface(&vein.veinInterface);
    sourceFacade.setStatusPollTime(10000); // never

    TimeMachineObject::feedEventLoop(); // initial processing
    JsonParamApi paramApi;
    paramApi.setParams(vein.veinDeviceParameter.getValue().toJsonObject());
    paramApi.setOn(true);
    QJsonObject jsonLoadParamsOn = paramApi.getParams();
    sourceFacade.switchLoad(jsonLoadParamsOn);

    TimeMachineForTest::getInstance()->processTimers(50);
    PersistentJsonState persistentState(JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties()));
    JsonParamApi paramApiDefault = persistentState.loadJsonState();
    QJsonObject jsonLoadParamsDefault = paramApiDefault.getParams();

    QCOMPARE(loadsReceived.count(), 1);
    QCOMPARE(loadsReceived[0], jsonLoadParamsDefault);

    QCOMPARE(statesReceived.count(), 3);
    QCOMPARE(statesReceived[0]["busy"], false);
    QCOMPARE(statesReceived[1]["busy"], true);
    QCOMPARE(statesReceived[2]["busy"], false);

    QCOMPARE(statesReceived[0]["errors"].toArray().count(), 0);
    QCOMPARE(statesReceived[1]["errors"].toArray().count(), 0);
    QCOMPARE(statesReceived[2]["errors"].toArray().count(), 1);
}

void test_sourcedevicefacade::checkVeinStateError()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    IoDeviceDemo* demoIO = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIO->setAllTransfersError(true);
    demoIO->setResponseDelay(false, 1); // at least our vein stub gets confused on high fire
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties());

    VeinComponentSetNotifier veinEventSystem(defaultEntityId);
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameParLoad, &vein.veinDeviceParameter);
    veinEventSystem.addComponentToNotify(componentNameActState, &vein.veinActDeviceState);
    QList<QJsonObject> loadsReceived;
    QList<QJsonObject> statesReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QJsonObject json = newValue.toJsonObject();
        if(componentName == componentNameParLoad) {
            loadsReceived.append(json);
        }
        if(componentName == componentNameActState) {
            statesReceived.append(json);
        }
    });

    SourceDeviceExtSerial sourceFacade(ioDevice, DefaultTestSourceProperties());
    sourceFacade.setVeinInterface(&vein.veinInterface);
    sourceFacade.setStatusPollTime(0);

    TimeMachineForTest::getInstance()->processTimers(50);
    PersistentJsonState persistentState(JsonStructureLoader::loadJsonStructure(DefaultTestSourceProperties()));
    JsonParamApi paramApiDefault = persistentState.loadJsonState();
    QJsonObject jsonLoadParamsDefault = paramApiDefault.getParams();

    QCOMPARE(loadsReceived.count(), 1);
    QCOMPARE(loadsReceived[0], jsonLoadParamsDefault);

    QCOMPARE(statesReceived.count(), 2);
    QCOMPARE(statesReceived[0]["busy"], false);
    QCOMPARE(statesReceived[1]["busy"], false);

    QCOMPARE(statesReceived[0]["errors"].toArray().count(), 0);
    QCOMPARE(statesReceived[1]["errors"].toArray().count(), 1);
}

