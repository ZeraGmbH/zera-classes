#ifndef CSOURCEACTION_H
#define CSOURCEACTION_H

#include <QMap>
#include <QJsonObject>

class cSourceActionTypes
{
public:
    enum ActionTypes {
        SET_RMS = 0,
        SET_ANGLE,
        SET_FREQUNCY,
        SET_HARMONICS,
        SET_REGULATOR,
        SWITCH_PHASES,
        LOAD_ACTION_COUNT,

        PERIODIC_FIRST = LOAD_ACTION_COUNT,
        QUERY_STATUS = PERIODIC_FIRST,
        PERIODIC_LAST
    };
    static int getLoadActionTypeCount() { return LOAD_ACTION_COUNT; }
    static int getPeriodicActionTypeCount() { return PERIODIC_LAST - PERIODIC_FIRST; }
};


class cSourceAction
{
public:
    cSourceAction();
    cSourceAction(const QJsonObject jsonSourceParamStructure, const QJsonObject jsonSourceParamState);
private:
    QJsonObject m_jsonSourceParamStructure;
    QJsonObject m_jsonSourceParamState;
};


typedef QMap<cSourceActionTypes::ActionTypes, cSourceAction> tSourceActionMap;

class cSourceActionGenerator
{
public:
    static tSourceActionMap GenerateLoadActionMap(const QJsonObject jsonSourceParamStructure, const QJsonObject jsonSourceParamState);
    static tSourceActionMap GeneratePeriodicActionMap(const QJsonObject jsonSourceParamStructure);
};

#endif // CSOURCEACTION_H
