#include "sourceactions.h"

bool cSourceActionTypes::isValidType(cSourceActionTypes::ActionTypes type)
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
        list.append(cSourceActionTypes::ActionTypes::SWITCH_PHASES);
    }
    else {
        for(int type=cSourceActionTypes::firstSwitchType; type<=cSourceActionTypes::lastSwitchType; ++type) {
            list.append(cSourceActionTypes::ActionTypes(type));
        }
    }
    return list;
}

tSourceActionTypeList cSourceActionGenerator::generatePeriodicActions()
{
    tSourceActionTypeList list;
    for(int type=cSourceActionTypes::firstPeriodicType; type<=int(cSourceActionTypes::lastPeriodicType); ++type) {
        list.append(cSourceActionTypes::ActionTypes(type));
    }
    return list;
}
