#ifndef ZERA_JSON_PARAMS_STRUCTURE_H
#define ZERA_JSON_PARAMS_STRUCTURE_H

#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>

class cZeraJsonParamsStructure
{
public:
    enum errorTypes {
        ERR_FILE_IO = 0,
        ERR_INVALID_JSON,

        ERR_INVALID_PARAM_TEMPLATE,
        ERR_INVALID_PARAM_TEMPLATE_DEFINITION,
        ERR_INVALID_PARAM_DEFINITION,
    };
    struct errEntry {
        errEntry(enum errorTypes errType, QString strInfo);
        enum errorTypes m_errType;
        QString m_strInfo;
    };

    typedef QList<errEntry> ErrList;

    cZeraJsonParamsStructure();

    ErrList loadJson(const QByteArray& jsonStructure, const QByteArray& jsonParamState,
                     const QString &jsonStructureErrHint = QString(), const QString &jsonParamStateErrHint = QString());
    ErrList loadJsonFromFiles(const QString& filenameJsonStructure, const QString& filenameJsonParamState);

    QByteArray exportJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact);
    ErrList exportJsonFile(const QString& filenameJsonStructure, QJsonDocument::JsonFormat format = QJsonDocument::Compact);

private:
    void resolveJsonParamTemplates(QJsonObject& jsonStructObj, ErrList& errList);
    bool resolveJsonParamTemplatesRecursive(QJsonObject& jsonStructObj, const QJsonObject jsonParamTemplatesObj, ErrList& errList);
    void validateParamData(QJsonObject::ConstIterator iter, bool inTemplate, ErrList& errList);
    void validateResolvedParamDataRecursive(QJsonObject& jsonStructObj, ErrList& errList);

    QJsonDocument m_jsonStructure;
    static QSet<QString> m_svalidParamEntryKeys;
    static QHash<QString, QStringList> m_svalidParamTypes;
};

#endif // ZERA_JSON_PARAMS_STRUCTURE_H
