#include "main-unittest-qt.h"
#include "qt-unittest-sourcedevicefacade.h"
#include "veincomponentsetnotifier-forunittest.h"
#include "source-device/sourcedevicefacade.h"
#include "vein/sourceveininterface.h"
#include "io-device/iodevicefactory.h"
#include "json/jsonstructureloader.h"
#include "json/persistentjsonstate.h"
#include <veinmoduleparameter.h>
#include <veinmoduleactvalue.h>
#include <jsonparamvalidator.h>

#include <QString>

static QObject* pSourceDeviceManagerTest = addTest(new SourceDeviceFacadeTest);

void SourceDeviceFacadeTest::init()
{
}

void SourceDeviceFacadeTest::checkDeviceInfo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    QCOMPARE(ioDevice->getDeviceInfo(), sourceFasade.getIoDeviceInfo());
}

void SourceDeviceFacadeTest::checkDemoOn()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    QCOMPARE(sourceFasade.hasDemoIo(), true);
}

void SourceDeviceFacadeTest::checkDemoOff()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    QCOMPARE(sourceFasade.hasDemoIo(), false);
}

static const QString componentNameActState = QStringLiteral("act_state");
static const QString componentNameActInfo = QStringLiteral("act_info");
static const QString componentNameParLoad = QStringLiteral("par_load");
static const QString noDescription = QStringLiteral("no-description");

struct TVeinObjects
{
    SourceVeinInterface veinInterface;
    cVeinModuleActvalue veinActDeviceState;
    cVeinModuleActvalue veinActDeviceInfo;
    cVeinModuleParameter veinDeviceParameter;
    cJsonParamValidator veinDeviceParameterValidator;
    TVeinObjects(QJsonObject jsonStructure, VeinEvent::EventSystem *veinEventSystem) :
        veinActDeviceState(VeinComponentSetNotifier::entityId,
                           veinEventSystem,
                           componentNameActState, noDescription, QVariant()),
        veinActDeviceInfo(VeinComponentSetNotifier::entityId,
                          veinEventSystem,
                          componentNameActInfo, noDescription, QVariant()),
        veinDeviceParameter(VeinComponentSetNotifier::entityId,
                            veinEventSystem,
                            componentNameParLoad, noDescription, QVariant())
    {
        veinInterface.setVeinDeviceState(&veinActDeviceState);
        veinInterface.setVeinDeviceInfo(&veinActDeviceInfo);
        veinInterface.setVeinDeviceParameter(&veinDeviceParameter);
        veinInterface.setVeinDeviceParameterValidator(&veinDeviceParameterValidator);
        veinDeviceParameterValidator.setJSonParameterStructure(jsonStructure);
    }
};

void SourceDeviceFacadeTest::checkVeinInitialStatus()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(sourceProperties);

    VeinComponentSetNotifier veinEventSystem;
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameActState, &vein.veinActDeviceState);
    QList<QJsonObject> statesReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameActState);
        QJsonObject jsonState = newValue.toJsonObject();
        statesReceived.append(jsonState);
    });

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    sourceFasade.setVeinInterface(&vein.veinInterface);

    QTest::qWait(shortQtEventTimeout);
    QJsonObject jsonStatusVein = vein.veinActDeviceState.getValue().toJsonObject();
    JsonDeviceStatusApi statusApi;
    statusApi.setDeviceInfo(deviceInfo);
    QJsonObject jsonStatusDefault = statusApi.getJsonStatus();
    QCOMPARE(jsonStatusVein, jsonStatusDefault);
    QCOMPARE(statesReceived.count(), 1);
    QCOMPARE(statesReceived[0], jsonStatusDefault);
}

void SourceDeviceFacadeTest::checkVeinInitialInfo()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(sourceProperties);

    VeinComponentSetNotifier veinEventSystem;
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameActInfo, &vein.veinActDeviceInfo);
    QList<QJsonObject> infosReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameActInfo);
        QJsonObject jsonInfo = newValue.toJsonObject();
        infosReceived.append(jsonInfo);
    });

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    sourceFasade.setVeinInterface(&vein.veinInterface);

    QTest::qWait(shortQtEventTimeout);
    QJsonObject jsonStructInfo = vein.veinActDeviceInfo.getValue().toJsonObject();
    QCOMPARE(jsonStructInfo, jsonStructure);
    QCOMPARE(infosReceived.count(), 1);
    QCOMPARE(infosReceived[0], jsonStructure);
}

void SourceDeviceFacadeTest::checkVeinInitialLoad()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(sourceProperties);

    VeinComponentSetNotifier veinEventSystem;
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameParLoad, &vein.veinDeviceParameter);
    QList<QJsonObject> loadsReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameParLoad);
        QJsonObject jsonInfo = newValue.toJsonObject();
        loadsReceived.append(jsonInfo);
    });

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    sourceFasade.setVeinInterface(&vein.veinInterface);

    QTest::qWait(shortQtEventTimeout);
    QJsonObject jsonLoadParams = vein.veinDeviceParameter.getValue().toJsonObject();
    PersistentJsonState persistentState(sourceProperties);
    JsonParamApi paramApi = persistentState.loadJsonState();
    QJsonObject jsonLoadParamsDefault = paramApi.getParams();
    QCOMPARE(jsonLoadParams, jsonLoadParamsDefault);
    QCOMPARE(loadsReceived.count(), 1);
    QCOMPARE(loadsReceived[0], jsonLoadParamsDefault);
}

void SourceDeviceFacadeTest::checkVeinSwitchTwoStateChanges()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(sourceProperties);

    VeinComponentSetNotifier veinEventSystem;
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameActState, &vein.veinActDeviceState);
    QList<QJsonObject> statesReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameActState);
        QJsonObject jsonState = newValue.toJsonObject();
        statesReceived.append(jsonState);
    });

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    sourceFasade.setVeinInterface(&vein.veinInterface);

    QTest::qWait(shortQtEventTimeout); // initial vein processing
    JsonParamApi paramApi;
    paramApi.setParams(vein.veinDeviceParameter.getValue().toJsonObject());
    paramApi.setOn(true);
    sourceFasade.switchLoad(paramApi.getParams());

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(statesReceived.count(), 3);
    QCOMPARE(statesReceived[0]["busy"], false);
    QCOMPARE(statesReceived[1]["busy"], true);
    QCOMPARE(statesReceived[2]["busy"], false);
}

void SourceDeviceFacadeTest::checkVeinSwitchChangesLoad()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(sourceProperties);

    VeinComponentSetNotifier veinEventSystem;
    TVeinObjects vein(jsonStructure, &veinEventSystem);
    veinEventSystem.addComponentToNotify(componentNameParLoad, &vein.veinDeviceParameter);
    QList<QJsonObject> loadsReceived;
    connect(&veinEventSystem, &VeinComponentSetNotifier::sigComponentChanged, [&] (QString componentName, QVariant newValue) {
        QCOMPARE(componentName, componentNameParLoad);
        QJsonObject jsonInfo = newValue.toJsonObject();
        loadsReceived.append(jsonInfo);
    });

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    sourceFasade.setVeinInterface(&vein.veinInterface);

    QTest::qWait(shortQtEventTimeout); // initial vein processing
    JsonParamApi paramApi;
    paramApi.setParams(vein.veinDeviceParameter.getValue().toJsonObject());
    paramApi.setOn(true);
    QJsonObject jsonLoadParamsOn = paramApi.getParams();
    sourceFasade.switchLoad(jsonLoadParamsOn);

    QTest::qWait(shortQtEventTimeout);
    PersistentJsonState persistentState(sourceProperties);
    JsonParamApi paramApiDefault = persistentState.loadJsonState();
    QJsonObject jsonLoadParamsDefault = paramApiDefault.getParams();

    QCOMPARE(loadsReceived.count(), 2);
    QCOMPARE(loadsReceived[0], jsonLoadParamsDefault);
    QCOMPARE(loadsReceived[1], jsonLoadParamsOn);
}

void SourceDeviceFacadeTest::checkVeinSwitchError()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    IoDeviceDemo* demoIO = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIO->setAllTransfersError(true);
    demoIO->setResponseDelay(false, 1); // at least our vein stub gets confused on high fire
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(sourceProperties);

    VeinComponentSetNotifier veinEventSystem;
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

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    sourceFasade.setVeinInterface(&vein.veinInterface);
    sourceFasade.setStatusPollTime(10000); // never

    QTest::qWait(shortQtEventTimeout); // initial processing
    JsonParamApi paramApi;
    paramApi.setParams(vein.veinDeviceParameter.getValue().toJsonObject());
    paramApi.setOn(true);
    QJsonObject jsonLoadParamsOn = paramApi.getParams();
    sourceFasade.switchLoad(jsonLoadParamsOn);

    QTest::qWait(50);
    PersistentJsonState persistentState(sourceProperties);
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

void SourceDeviceFacadeTest::checkVeinStateError()
{
    QString deviceInfo = "__foo__";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice(deviceInfo);
    IoDeviceDemo* demoIO = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIO->setAllTransfersError(true);
    demoIO->setResponseDelay(false, 1); // at least our vein stub gets confused on high fire
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    QJsonObject jsonStructure = JsonStructureLoader::loadJsonStructure(sourceProperties);

    VeinComponentSetNotifier veinEventSystem;
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

    SourceDeviceFacade sourceFasade(ioDevice, sourceProperties);
    sourceFasade.setVeinInterface(&vein.veinInterface);
    sourceFasade.setStatusPollTime(0);

    QTest::qWait(50);
    PersistentJsonState persistentState(sourceProperties);
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

