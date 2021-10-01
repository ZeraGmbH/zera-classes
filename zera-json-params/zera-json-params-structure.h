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
    cZeraJsonParamsStructure();

    enum errorTypes {
        ERR_INVALID_JSON = 0,

        ERR_INVALID_PARAM_TEMPLATE,
        ERR_INVALID_PARAM_TEMPLATE_DEFINITION,
        ERR_INVALID_PARAM_DEFINITION,
    };
    struct errEntry {
        errEntry(enum errorTypes errType, QString strInfo);
        QString strID();
        enum errorTypes m_errType;
        QString m_strInfo;
    };
    typedef QList<errEntry> ErrList;

    bool isValid();

    ErrList loadJson(const QByteArray& jsonData, const QString &errHint = QString());
    QByteArray exportJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact);

    QByteArray createDefaultJsonState(QJsonDocument::JsonFormat format = QJsonDocument::Compact);
    ErrList validateJsonState(const QByteArray& jsonData);

private:
    void resolveJsonParamTemplates(QJsonObject& jsonStructObj, ErrList& errList);
    bool resolveJsonParamTemplatesRecursive(QJsonObject& jsonStructObj, const QJsonObject jsonParamTemplatesObj, ErrList& errList);
    void validateParamData(QJsonObject::ConstIterator iter, bool inTemplate, QStringList jsonStructurePathList, ErrList& errList);
    void validateResolvedParamDataRecursive(QJsonObject& jsonStructObj, QStringList jsonStructurePathList, ErrList& errList);
    void createDefaultJsonStateRecursive(QJsonObject& jsonStateObj, QJsonObject &jsonStructObj, QStringList jsonStructurePathList);

    QJsonObject m_jsonObjStructure;
    static QSet<QString> m_svalidParamEntryKeys;
    static QHash<QString, QStringList> m_svalidParamTypes;
};

#endif // ZERA_JSON_PARAMS_STRUCTURE_H
