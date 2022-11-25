#include "adjustmentmoduleactivator.h"
#include "adjustmentmoduleconfiguration.h"
#include "test_adjustmentmoduleactivator.h"
#include "vs_veinhash.h"
#include <QTest>
#include <QFile>
#include <QByteArray>

QTEST_MAIN(test_adjustmentmoduleactivator)

void test_adjustmentmoduleactivator::instanciate()
{
    using namespace ADJUSTMENTMODULE;
    AdjustmentModuleActivateData activationData;
    Zera::Proxy::cProxy* proxy= Zera::Proxy::cProxy::getInstance();

    std::shared_ptr<cBaseModuleConfiguration> pConfiguration = std::make_shared<cAdjustmentModuleConfiguration>();

    QString configPath = QString(MODMAN_CONFIG_PATH);
    QFile xmlFile(configPath + "/mt310s2-adjustmentmodule.xml");
    xmlFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
    QByteArray xmlConfigData = xmlFile.readAll();
    pConfiguration->setConfiguration(xmlConfigData); // trouble loading xsd

    VeinStorage::VeinHash storagesystem;
    cAdjustmentModule module(1, proxy, 3, &storagesystem);

    AdjustmentModuleActivator obj(&module, proxy, pConfiguration, activationData);
    obj.setupServerResponseHandlers();
}

