#ifndef POWER1DSPCMDGENERATOR_H
#define POWER1DSPCMDGENERATOR_H

#include "power1moduleconfigdata.h"
#include "meassytemchannels.h"
#include <dspchainidgen.h>
#include "measmode.h"
#include "foutinfo.h"
#include <QStringList>

class Power1DspCmdGenerator
{
public:
    static QStringList getCmdsInitVars(std::shared_ptr<MeasMode> initialMMode, int samplesPerPeroid, double integrationTime, bool startTiTime, DspChainIdGen &idGen);
    static QStringList getCmdsInitOutputVars();
    static QStringList getCmdsMModeMQREF(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsMMode4LBK(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsMMode3LW(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsMMode3LB(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsMMode3LS(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsMMode3LSg(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsMModeXLW(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsMModeXLB(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsMModeXLS(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsMModeXLSg(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen& idGen);
    static QStringList getCmdsSumAndAverage(DspChainIdGen &idGen);
    static QStringList getCmdsFreqOutput(const POWER1MODULE::cPower1ModuleConfigData *configData,
                                         const QMap<QString, cFoutInfo> &foutInfoMap,
                                         int irqNo, DspChainIdGen& idGen);
private:
    static QStringList getCmdsSkipOnUnequal(const QString& varName, int varValue, quint16 chainId);
    static QStringList getCmdsSkipOnMModeNotSelected(int dspSelectCode, quint16 chainId);
    static QStringList getCmdsSkipOnPhaseNotSelected(int phase, quint16 chainId);
};

#endif // POWER1DSPCMDGENERATOR_H
