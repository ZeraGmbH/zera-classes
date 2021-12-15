#include "sourceactions.h"

bool SourceActionTypes::isValidType(SourceActionTypes::ActionTypes type)
{
    return
            (type >= firstSwitchType && type <= lastSwitchType) ||
            (type >= firstPeriodicType && type <= lastPeriodicType);
}

tSourceActionTypeList cSourceActionGenerator::generateSwitchActions(SourceJsonParamApi requestedParams)
{
    tSourceActionTypeList list;
    // 1st guess: switch off just switch phases
    if(!requestedParams.getOn()) {
        list.append(SourceActionTypes::ActionTypes::SWITCH_PHASES);
    }
    else {
        for(int type=SourceActionTypes::firstSwitchType; type<=SourceActionTypes::lastSwitchType; ++type) {
            list.append(SourceActionTypes::ActionTypes(type));
        }
    }
    return list;
}

tSourceActionTypeList cSourceActionGenerator::generatePeriodicActions()
{
    tSourceActionTypeList list;
    for(int type=SourceActionTypes::firstPeriodicType; type<=int(SourceActionTypes::lastPeriodicType); ++type) {
        list.append(SourceActionTypes::ActionTypes(type));
    }
    return list;
}
