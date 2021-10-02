#ifndef ZERA_JSON_PARAMS_STATE_H
#define ZERA_JSON_PARAMS_STATE_H

#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QJsonObject>

class cZeraJsonParamsState
{
public:
    cZeraJsonParamsState();

    bool isValid();

    void setState(const QJsonObject& jsonState);
    void mergeState(const QJsonObject& jsonState);
    const QJsonObject& state();
private:
    QJsonObject m_jsonObjState;
};

#endif // ZERA_JSON_PARAMS_STATE_H
