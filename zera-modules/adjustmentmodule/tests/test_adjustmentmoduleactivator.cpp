#include "adjustmentmoduleactivator.h"
#include "test_adjustmentmoduleactivator.h"
#include "vs_veinhash.h"
#include <QTest>

QTEST_MAIN(test_adjustmentmoduleactivator)

void test_adjustmentmoduleactivator::instanciate()
{
    using namespace ADJUSTMENTMODULE;
    AdjustmentModuleActivateData activationData;
    Zera::Proxy::cProxy* proxy= Zera::Proxy::cProxy::getInstance();
    std::shared_ptr<cBaseModuleConfiguration> pConfiguration;
    VeinStorage::VeinHash storagesystem;
    cAdjustmentModule module(1, proxy, 3, &storagesystem);

    AdjustmentModuleActivator obj(&module, proxy, pConfiguration, activationData);
    obj.setupServerResponseHandlers();
}

