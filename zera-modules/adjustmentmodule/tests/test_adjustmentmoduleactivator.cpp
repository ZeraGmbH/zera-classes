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
    AdjustmentModuleCommonPtr activationData = std::make_shared<AdjustmentModuleCommon>();

    /*std::shared_ptr<cAdjustmentModuleConfiguration> pConfiguration = std::make_shared<cAdjustmentModuleConfiguration>();

    QString configPath = QString(MODMAN_CONFIG_PATH);
    QString xsdpath = configPath+ "/adjustmentmodule.xsd";

    QFile xmlFile(configPath + "/mt310s2-adjustmentmodule.xml");
    xmlFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);

    QByteArray xmlConfigData = xmlFile.readAll();
    pConfiguration->setConfig(xmlConfigData, xsdpath);

    VeinStorage::VeinHash storagesystem; */
    cSocket rmSocket;
    cSocket pcbSocket;
    AdjustmentModuleActivator obj(QStringList() << "m0" << "m1"  << "m2",
                                  activationData);
}
