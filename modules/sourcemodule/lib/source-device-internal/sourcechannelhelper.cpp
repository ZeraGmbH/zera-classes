#include "sourcechannelhelper.h"

QStringList SourceChannelHelper::getChannelMNamesSwitchedOn(const JsonStructApi &sourceCapabilities,
                                                            const JsonParamApi &wantedLoadpoint)
{
    QStringList activeModeOnChannelMNames;
    for (phaseType type : {phaseType::U, phaseType::I}) {
        const int phaseCount = type==phaseType::U ?
                                   sourceCapabilities.getCountUPhases() :
                                   sourceCapabilities.getCountIPhases();
        for (int phaseNo=0; phaseNo < phaseCount; ++phaseNo) {
            if(wantedLoadpoint.getOn() && wantedLoadpoint.getOn(type, phaseNo))
                activeModeOnChannelMNames.append(getChannelMName(type, phaseNo));
        }
    }
    return activeModeOnChannelMNames;
}

QString SourceChannelHelper::getChannelMName(phaseType type, int phaseNoBase0)
{
    switch (phaseNoBase0) {
    case 0:
        return type == phaseType::U ? "m0" : "m3";
    case 1:
        return type == phaseType::U ? "m1" : "m4";
    case 2:
        return type == phaseType::U ? "m2" : "m5";
    }
    qCritical("getChannelMName: invalid parameters!");
    return "m0";
}

QString SourceChannelHelper::getAlias(phaseType type, int phaseNoBase0)
{
    switch (phaseNoBase0) {
    case 0:
        return type == phaseType::U ? "UL1" : "IL1";
    case 1:
        return type == phaseType::U ? "UL2" : "IL2";
    case 2:
        return type == phaseType::U ? "UL3" : "IL3";
    }
    qCritical("getChannelMName: invalid parameters!");
    return "m0";
}
