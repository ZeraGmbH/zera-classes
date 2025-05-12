#include "test_api_trust_authentication.h"

#include <QTest>

#include "mocktcpnetworkfactory.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"
#include "vf_client_component_setter.h"
#include "vf_client_rpc_invoker.h"

#define API_ENTITY_ID 1500

const QString trustPath("/tmp/trustlist.json");

QTEST_MAIN(test_api_trust_authentication)

void test_api_trust_authentication::initTestCase()
{
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();

    TimerFactoryQtForTest::enableTest();
}

void test_api_trust_authentication::init()
{
    QFile::remove(trustPath);

    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    m_cmdHandler = VfCmdEventHandlerSystem::create();

    m_modmanSetupFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());
    m_modmanSetupFacade->addSubsystem(m_cmdHandler.get());

    m_modMan = std::make_unique<TestModuleManager>(
        m_modmanSetupFacade.get(),
        m_serviceInterfaceFactory,
        m_tcpFactory
    );

    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();

    TimeMachineForTest::reset();

    m_modMan->loadSession(":/session-minimal.json");
    m_modMan->waitUntilModulesAreReady();

    m_pApiModule = static_cast<APIMODULE::cApiModule*>(m_modMan->getModule("apimodule", API_ENTITY_ID));

    m_pApiModule->setTrustListPath(trustPath);

    TimeMachineObject::feedEventLoop();
}

void test_api_trust_authentication::setVfComponent(const QString& componentName, const QVariant& newValue)
{
    QVariant oldValue = getVfComponent(componentName);
    QEvent* event = VfClientComponentSetter::generateEvent(API_ENTITY_ID, componentName, oldValue, newValue);

    emit m_modmanSetupFacade->getStorageSystem()->sigSendEvent(event);

    TimeMachineObject::feedEventLoop();
}

QVariant test_api_trust_authentication::getVfComponent(const QString& componentName)
{
    return m_modmanSetupFacade->getStorageSystem()->getDb()->getStoredValue(API_ENTITY_ID, componentName);
}

void test_api_trust_authentication::rpcRequestWillTriggerGUI() {
    // Should be idle.
    QVERIFY(getVfComponent("ACT_TrustList").toList().isEmpty());
    QCOMPARE(getVfComponent("ACT_PendingRequest"), QJsonObject());
    QCOMPARE(getVfComponent("ACT_TLChangeCount"), 1);
    QCOMPARE(getVfComponent("PAR_GuiDialogFinished"), false);
    QCOMPARE(getVfComponent("PAR_ReloadTrustList"), false);

    // Fire up RPC.
    QVariantMap args;
    args["p_displayName"] = "Karl";
    args["p_token"] = "-- PEM --";
    args["p_tokenType"] = "PublicKey";

    VfClientRPCInvokerPtr rpc = VfClientRPCInvoker::create(API_ENTITY_ID);

    m_cmdHandler->addItem(rpc);

    rpc->setEventSystem(m_modmanSetupFacade->getSystemModuleEventSystem());

    QUuid resultId;
    QVariantMap result;

    connect(rpc.get(), &VfClientRPCInvoker::sigRPCFinished, [&resultId, &result](bool ok, QUuid identifier, const QVariantMap &resultData) {
        QVERIFY(ok);

        resultId = identifier;
        result = resultData;
    });

    const QUuid id(rpc->invokeRPC("RPC_Authenticate", args));

    TimeMachineObject::feedEventLoop();

    // Should have fired start request.
    QJsonObject trust;
    trust["name"] = "Karl";
    trust["tokenType"] = "PublicKey";
    trust["token"] = "-- PEM --";

    QCOMPARE(getVfComponent("ACT_PendingRequest"), trust);

    // RPC should not be done.
    QVERIFY(resultId.isNull());
    QVERIFY(result.isEmpty());

    // Update trust file and simulate UI response
    {
        QJsonArray trusts;
        trusts.append(trust);
        QJsonDocument doc(trusts);

        QFile trustFile(trustPath);
        QVERIFY(trustFile.open(QIODevice::WriteOnly));

        trustFile.resize(0);
        trustFile.write(doc.toJson(QJsonDocument::Indented));
    }

    setVfComponent("PAR_GuiDialogFinished", true);

    // RPC should be done.
    QCOMPARE(resultId, id);
    QCOMPARE(result["RemoteProcedureData::Return"], true);

    // Should have completed.
    QVariantList actTrusts(getVfComponent("ACT_TrustList").toList());

    QCOMPARE(actTrusts.length(), 1);
    QCOMPARE(actTrusts[0], trust);
    QCOMPARE(getVfComponent("ACT_PendingRequest"), QJsonObject());
    QCOMPARE(getVfComponent("ACT_TLChangeCount"), 2);
    QCOMPARE(getVfComponent("PAR_GuiDialogFinished"), true);
}
