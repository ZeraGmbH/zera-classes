#include "adjustmentmoduleactivator.h"
#include "adjustmentmoduleconfiguration.h"
#include "test_adjustmentmoduleactivator.h"
#include "vs_veinhash.h"
#include <QTest>
#include <QFile>
#include <QByteArray>

QTEST_MAIN(test_adjustmentmoduleactivator);

void test_adjustmentmoduleactivator::instantiate()
{
    using namespace ADJUSTMENTMODULE;
    AdjustmentModuleActivateDataPtr activationData = std::make_shared<AdjustmentModuleActivateData>();
    Zera::Proxy::cProxy* proxy= Zera::Proxy::cProxy::getInstance();

    std::shared_ptr<cAdjustmentModuleConfiguration> pConfiguration = std::make_shared<cAdjustmentModuleConfiguration>();

    QString configPath = QString(MODMAN_CONFIG_PATH);
    QString xsdpath = configPath+ "/adjustmentmodule.xsd";

    QFile xmlFile(configPath + "/mt310s2-adjustmentmodule.xml");
    xmlFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);

    QByteArray xmlConfigData = xmlFile.readAll();
    pConfiguration->setConfig(xmlConfigData, xsdpath);

    VeinStorage::VeinHash storagesystem;
    cAdjustmentModule module(1, proxy, 3, &storagesystem);

    AdjustmentModuleActivator obj(&module, pConfiguration, activationData);
}
