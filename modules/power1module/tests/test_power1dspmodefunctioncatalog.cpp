#include "test_power1dspmodefunctioncatalog.h"
#include "power1dspmodefunctioncatalog.h"
#include <measmodeinfo.h>
#include <measmodecatalog.h>
#include <QTest>

QTEST_MAIN(test_power1dspmodefunctioncatalog)

void test_power1dspmodefunctioncatalog::allAvailableModesHandledByCatalog()
{
    // Power1-module supports all measurement modes so all must be in catalog
    // Once modes for other modules are invented, we need to re-think
    MeasSystemChannels measChannelPairList(MeasPhaseCount);
    DspChainIdGen dspDummyChainGen;
    MeasModeBroker measBroker(Power1DspModeFunctionCatalog::get(MeasPhaseCount), dspDummyChainGen);
    for(int modeId=MeasModeFirst; modeId<MeasModeCount; modeId++) {
        cMeasModeInfo info = MeasModeCatalog::getInfo(measmodes(modeId));
        MeasModeBroker::BrokerReturn brokerReturn = measBroker.getMeasMode(info.getName(), measChannelPairList);
        qInfo("Checking %s...", qPrintable(info.getName()));
        QVERIFY(brokerReturn.isValid());
    }
}
