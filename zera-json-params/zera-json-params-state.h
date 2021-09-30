#ifndef ZERA_JSON_PARAMS_STATE_H
#define ZERA_JSON_PARAMS_STATE_H

#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>

class cZeraJsonParamsState
{
public:
    enum errorTypes {
        ERR_FILE_IO = 0,
        ERR_INVALID_JSON,

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

    cZeraJsonParamsState();

    ErrList loadJson(const QByteArray& jsonStructure, const QByteArray& jsonParamState,
                     const QString &jsonStructureErrHint = QString(), const QString &jsonParamStateErrHint = QString());
    ErrList loadJsonFromFiles(const QString& filenameJsonStructure, const QString& filenameJsonParamState);

    QByteArray exportJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact);
    ErrList exportJsonFile(const QString& filenameJsonParamState, QJsonDocument::JsonFormat format = QJsonDocument::Compact);

    ErrList param(const QStringList &paramPath, QVariant& value);
    ErrList setParam(const QStringList& paramPath, QVariant value);

private:

    QJsonDocument m_jsonParamState;
};

#endif // ZERA_JSON_PARAMS_STATE_H
