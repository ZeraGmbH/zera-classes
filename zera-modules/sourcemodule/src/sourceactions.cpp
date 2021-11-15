#include "sourceactions.h"

tSourceActionTypeList cSourceActionGenerator::generateLoadActionList(cSourceJsonParamApi requestedParams)
{
    tSourceActionTypeList list;
    // 1st guess: switch off just switch phases
    if(!requestedParams.getOn()) {
        list.append(cSourceActionTypes::ActionTypes::SWITCH_PHASES);
    }
    else {
        for(int type=0; type<cSourceActionTypes::getLoadActionTypeCount(); ++type) {
            list.append(cSourceActionTypes::ActionTypes(type));
        }
    }
    return list;
}

tSourceActionTypeList cSourceActionGenerator::generatePeriodicActionList()
{
    tSourceActionTypeList list;
    for(int type=cSourceActionTypes::PERIODIC_FIRST; type<int(cSourceActionTypes::PERIODIC_LAST); ++type) {
        list.append(cSourceActionTypes::ActionTypes(type));
    }
    return list;
}

