#ifndef CSOURCEACTION_H
#define CSOURCEACTION_H

#include <QList>
#include <QJsonObject>
#include "sourcejsonapi.h"

class cSourceActionTypes
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
    static constexpr int firstSwitchType = int(SWITCH_UNDEF)+1;
    static constexpr int lastSwitchType = int(SWITCH_UNDEF2)-1;
    static constexpr int switchTypeCount = int(SWITCH_UNDEF2)-int(SWITCH_UNDEF)-1;

    static constexpr int firstPeriodicType = int(PERIODIC_UNDEF)+1;
    static constexpr int lastPeriodicType = int(PERIODIC_UNDEF2)-1;
    static constexpr int periodicTypeCount = int(PERIODIC_UNDEF2)-int(PERIODIC_UNDEF)-1;


    static constexpr int totalMinWithInvalid = int(SWITCH_UNDEF)+1;
    static constexpr int totalMaxWithInvalid = int(BEYOND_ALL)-1;
};


typedef QList<cSourceActionTypes::ActionTypes> tSourceActionTypeList;

class cSourceActionGenerator
{
public:
    static tSourceActionTypeList generateSwitchActions(cSourceJsonParamApi requestedParams);
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
