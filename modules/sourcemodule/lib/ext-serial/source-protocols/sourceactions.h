#ifndef CSOURCEACTION_H
#define CSOURCEACTION_H

#include <QList>
#include <QJsonObject>
#include "jsonparamapi.h"

class SourceActionTypes
{
public:
    enum ActionTypes {
        SWITCH_UNDEF = 0,
        SET_RMS,
        SET_ANGLE,
        SET_FREQUENCY,
        SET_HARMONICS,
        SET_REGULATOR,
        SWITCH_PHASES,
        SWITCH_UNDEF2,

        PERIODIC_UNDEF,
        QUERY_STATUS,
        QUERY_ACTUAL,
        PERIODIC_UNDEF2,

        BEYOND_ALL
    };

    static bool isValidType(ActionTypes type);
    static constexpr int firstSwitchType = int(SWITCH_UNDEF)+1;
    static constexpr int lastSwitchType = int(SWITCH_UNDEF2)-1;
    static constexpr int switchTypeCount = int(SWITCH_UNDEF2)-int(SWITCH_UNDEF)-1;

    static constexpr int firstPeriodicType = int(PERIODIC_UNDEF)+1;
    static constexpr int lastPeriodicType = int(PERIODIC_UNDEF2)-1;
    static constexpr int periodicTypeCount = int(PERIODIC_UNDEF2)-int(PERIODIC_UNDEF)-1;


    static constexpr int totalMinWithInvalid = int(SWITCH_UNDEF);
    static constexpr int totalMaxWithInvalid = int(BEYOND_ALL);
};


typedef QList<SourceActionTypes::ActionTypes> tSourceActionTypeList;

class SourceActionGenerator
{
public:
    static tSourceActionTypeList generateSwitchActions(JsonParamApi requestedParams);
    static tSourceActionTypeList generatePeriodicActions();
};


template<class TListForSort>
TListForSort sortListCustom(TListForSort sourceList, const TListForSort sortList, bool toFront)
{
    TListForSort sortedList;
    for(auto sortEntry : sortList) { // apply sortList
        int sortEntryIndex = sourceList.indexOf(sortEntry);
        if(sortEntryIndex >= 0) {
            sortedList.append(sortEntry);
            sourceList.removeAt(sortEntryIndex);
        }
    }
    if(toFront) {
        sortedList = sortedList + sourceList;
    }
    else {
        sortedList = sourceList + sortedList;
    }
    return sortedList;
}

#endif // CSOURCEACTION_H
