#include "adjustmentmoduleactivator.h"
#include "adjustmentmoduleconfiguration.h"
#include "test_adjustmentmoduleactivator.h"
#include "vs_veinhash.h"
#include <QFile>
#include <QByteArray>
#include <QTest>

QTEST_MAIN(test_adjustmentmoduleactivator)

void test_adjustmentmoduleactivator::instantiate()
{
    AdjustmentModuleCommonPtr activationData = std::make_shared<AdjustmentModuleCommon>();

    /*std::shared_ptr<cAdjustmentModuleConfiguration> pConfiguration = std::make_shared<cAdjustmentModuleConfiguration>();

    QString configPath = QString(CONFIG_SOURCE_PATH);
    QString xsdpath = configPath+ "/adjustmentmodule.xsd";

    QFile xmlFile(configPath + "/mt310s2-adjustmentmodule.xml");
    xmlFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);

    QByteArray xmlConfigData = xmlFile.readAll();
    pConfiguration->validateAndSetConfig(xmlConfigData, xsdpath);

    VeinStorage::VeinHash storagesystem; */
    cSocket rmSocket;
    cSocket pcbSocket;
    AdjustmentModuleActivator obj(QStringList() << "m0" << "m1"  << "m2",
                                  activationData, true);
}
