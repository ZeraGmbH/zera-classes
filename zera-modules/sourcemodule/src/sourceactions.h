#ifndef CSOURCEACTION_H
#define CSOURCEACTION_H

#include <QList>
#include <QJsonObject>

class cSourceActionTypes
{
public:
    enum ActionTypes {
        SET_RMS = 0,
        SET_ANGLE,
        SET_FREQUENCY,
        SET_HARMONICS,
        SET_REGULATOR,
        SWITCH_PHASES,
        LOAD_ACTION_COUNT,

        PERIODIC_FIRST = LOAD_ACTION_COUNT,
        QUERY_STATUS = PERIODIC_FIRST,
        QUERY_ACTUAL,
        PERIODIC_LAST
    };
    static int getLoadActionTypeCount() { return LOAD_ACTION_COUNT; }
    static int getPeriodicActionTypeCount() { return PERIODIC_LAST - PERIODIC_FIRST; }
};


typedef QList<cSourceActionTypes::ActionTypes> tSourceActionTypeList;

class cSourceActionGenerator
{
public:
    static tSourceActionTypeList generateLoadActionList(const QJsonObject jsonSourceParamState);
    static tSourceActionTypeList generatePeriodicActionList();
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
