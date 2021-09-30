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

    ErrList loadJson(const QByteArray& jsonStructure, const QByteArray& jsonParamState,
                     const QString &jsonStructureErrHint = QString(), const QString &jsonParamStateErrHint = QString());
    ErrList loadJsonFromFiles(const QString& filenameJsonStructure, const QString& filenameJsonParamState);

    QByteArray exportJsonStructure(QJsonDocument::JsonFormat format = QJsonDocument::Compact);
    ErrList exportJsonStructureFile(const QString& filenameJsonStructure, QJsonDocument::JsonFormat format = QJsonDocument::Compact);
    QByteArray exportJsonParamState(QJsonDocument::JsonFormat format = QJsonDocument::Compact);
    ErrList exportJsonParamStateFile(const QString& filenameJsonParamState, QJsonDocument::JsonFormat format = QJsonDocument::Compact);

    ErrList param(const QStringList &paramPath, QVariant& value);
    ErrList setParam(const QStringList& paramPath, QVariant value);

private:
    void resolveJsonParamTemplates(QJsonObject& jsonStructObj, ErrList& errList);
    bool resolveJsonParamTemplatesRecursive(QJsonObject& jsonStructObj, const QJsonObject jsonParamTemplatesObj, ErrList& errList);
    void validateParamData(QJsonObject::ConstIterator iter, bool inTemplate, ErrList& errList);
    void validateResolvedParamDataRecursive(QJsonObject& jsonStructObj, ErrList& errList);

    QJsonDocument m_jsonStructure;
    QJsonDocument m_jsonParamState;
    static QSet<QString> m_svalidParamEntryKeys;
    static QHash<QString, QStringList> m_svalidParamTypes;
};

#endif // ZERA_JSON_PARAMS_H
