#ifndef ZERA_JSON_PARAMS_STATE_H
#define ZERA_JSON_PARAMS_STATE_H

#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
class cZeraJsonParamsStructure;

class cZeraJsonParamsState
{
public:
    cZeraJsonParamsState();

    enum errorTypes {
        ERR_INVALID_JSON = 0,
        ERR_INVALID_STRUCTURE,

        ERR_INVALID_PARAM_PATH,
        ERR_PARAM_DOES_NOT_EXIST,
        ERR_INVALID_VALUE
    };
    struct errEntry {
        errEntry(enum errorTypes errType, QString strInfo);
        enum errorTypes m_errType;
        QString m_strInfo;
    };
    typedef QList<errEntry> ErrList;

    bool isValid();

    ErrList loadJson(const QByteArray& jsonData, cZeraJsonParamsStructure* paramStructure, const QString &errHint = QString());
    QByteArray exportJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact);

    ErrList param(const QStringList &paramPath, QVariant& value);
    ErrList setParam(const QStringList& paramPath, QVariant value);

private:

    QJsonDocument m_jsonParamState;
    cZeraJsonParamsStructure* m_paramStructure = nullptr;
};

#endif // ZERA_JSON_PARAMS_STATE_H
