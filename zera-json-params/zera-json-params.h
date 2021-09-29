#ifndef ZERA_JSON_PARAMS_H
#define ZERA_JSON_PARAMS_H

#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>

class cZeraJsonParams
{
public:
    enum errorTypes {
        ERR_FILE_IO = 0,
        ERR_INVALID_JSON,
        ERR_INVALID_PARAM_TEMPLATE,
        ERR_INVALID_PARAM_TEMPLATE_DEFINITION,
        ERR_INVALID_PARAM_DEFINITION,
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

    cZeraJsonParams();

    ErrList loadJson(const QByteArray& jsonStructure, const QByteArray& jsonParams,
                     const QString &jsonStructureErrHint = QString(), const QString &jsonParamsErrHint = QString());
    ErrList loadJsonFromFiles(const QString& filenameJSONStructure, const QString& filenameJSONParams);

    QByteArray exportJson();
    ErrList exportJsonFile(const QString& filenameJSON);

    ErrList param(const QStringList &paramPath, QVariant& val);
    ErrList setParam(const QStringList& paramPath, QVariant value);

private:
    void resolveJSONParamTemplates(QJsonObject& jsonStructObj, ErrList& errList);
    bool resolveJSONParamTemplatesRecursive(QJsonObject& jsonStructObj, const QJsonObject jsonParamTemplatesObj, ErrList& errList);
    void validateParamData(QJsonObject::ConstIterator iter, bool inTemplate, ErrList& errList);
    void validateCompletParamDataRecursive(QJsonObject& jsonStructObj, ErrList& errList);

    QJsonDocument m_jsonStructure;
    QJsonDocument m_jsonParams;
    static QSet<QString> m_svalidParamEntryKeys;
    static QHash<QString, QStringList> m_svalidParamTypes;
};

#endif // ZERA_JSON_PARAMS_H
