#ifndef CSOURCEACTION_H
#define CSOURCEACTION_H

#include <QMap>
#include <QJsonObject>

class cSourceAction
{
public:
    enum ActionTypes {
        SET_RMS = 0,
        SET_ANGLE,
        SET_FREQUNCY,
        SET_HARMONICS,
        SET_REGULATOR,
        SWITCH_PHASES,

        ACTION_COUNT
    };
    enum PeriodicActionTypes {
        QUERY_STATUS = 0,

        PERIODIC_ACTION_COUNT
    };
    cSourceAction(const QJsonObject jsonSourceParamStructure, const QJsonObject jsonSourceParamState);
private:
    QJsonObject m_jsonSourceParamStructure;
    QJsonObject m_jsonSourceParamState;
};


typedef QMap<cSourceAction::ActionTypes, cSourceAction> tActionMap;
typedef QMap<cSourceAction::PeriodicActionTypes, cSourceAction> tPeriodicActionMap;

class cSourceActionGenerator
{
public:
    static tActionMap GenerateActionMap(const QJsonObject jsonSourceParamStructure, const QJsonObject jsonSourceParamState);
    static tPeriodicActionMap GeneratePeriodicActionMap(const QJsonObject jsonSourceParamStructure, const QJsonObject jsonSourceParamState);
};

#endif // CSOURCEACTION_H
