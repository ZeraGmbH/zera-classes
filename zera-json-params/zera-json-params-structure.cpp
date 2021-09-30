#include "zera-json-params-structure.h"
#include <QJsonObject>
#include <QSet>

cZeraJsonParamsStructure::cZeraJsonParamsStructure()
{
}

cZeraJsonParamsStructure::ErrList cZeraJsonParamsStructure::loadJson(const QByteArray &jsonData, const QString &errHint)
{
    ErrList errList;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if(jsonDoc.isNull() || !jsonDoc.isObject()) {
        errEntry error(ERR_INVALID_JSON, errHint.isEmpty() ? jsonData : errHint);
        errList.push_back(error);
    }
    // resolve param state templates & validate
    if(errList.isEmpty()) {
        QJsonObject jsonObj = jsonDoc.object();
        // Note: param templates are validated during resolve activity
        resolveJsonParamTemplates(jsonObj, errList);
        // now params are complete for validation
        QStringList jsonStructurePathList;
        validateResolvedParamDataRecursive(jsonObj, jsonStructurePathList, errList);
        if(errList.isEmpty()) {
            m_jsonObjStructure = jsonObj;
        }
    }
    return errList;
}

bool cZeraJsonParamsStructure::isValid()
{
    return !m_jsonObjStructure.isEmpty();
}

QByteArray cZeraJsonParamsStructure::exportJson(QJsonDocument::JsonFormat format)
{
    QByteArray jsonData = "{}";
    if(isValid()) {
        QJsonDocument jsonDoc;
        jsonDoc.setObject(m_jsonObjStructure);
        jsonData = jsonDoc.toJson(format);
    }
    return jsonData;
}

QByteArray cZeraJsonParamsStructure::createDefaultJsonState()
{

}

cZeraJsonParamsStructure::ErrList cZeraJsonParamsStructure::validateJsonState(const QByteArray &jsonData)
{

}

void cZeraJsonParamsStructure::resolveJsonParamTemplates(QJsonObject &jsonStructObj, ErrList& errList)
{
    // Find "param_templates" object and start recursive resolve
    QJsonValue paramTemplateValue = jsonStructObj["param_templates"];
    if(!paramTemplateValue.isNull()) { // param_templates can be ommitted
        if(paramTemplateValue.isObject()) {
            QJsonObject jsonParamTemplatesObj = paramTemplateValue.toObject();
            // validate param_templates
            for(QJsonObject::ConstIterator iter=jsonParamTemplatesObj.begin(); iter!=jsonParamTemplatesObj.end(); ++iter) {
                validateParamData(iter, true, QStringList()<<"param_templates", errList);
            }
            resolveJsonParamTemplatesRecursive(jsonStructObj, jsonParamTemplatesObj, errList);
            if(errList.isEmpty()) {
                jsonStructObj.remove("param_templates");
            }
        }
        else {
            errEntry error(ERR_INVALID_PARAM_TEMPLATE, QStringLiteral("param_templates"));
            errList.push_back(error);
        }
    }
}

bool cZeraJsonParamsStructure::resolveJsonParamTemplatesRecursive(QJsonObject& jsonStructObj, const QJsonObject jsonParamTemplatesObj, ErrList& errList)
{
    bool objectChanged = false;
    for(QJsonObject::Iterator sub=jsonStructObj.begin(); sub!=jsonStructObj.end(); sub++) {
        QString key = sub.key();
        if(key == QStringLiteral("param_template")) {
            QString linkTo = jsonStructObj[key].toString();
            if(!linkTo.isEmpty()) {
                QJsonValue specVal = jsonParamTemplatesObj[linkTo];
                // param_template reference found in param_templates?
                if(!specVal.isNull() && specVal.isObject()) {
                    QJsonObject spec = jsonParamTemplatesObj[linkTo].toObject();
                    for(QJsonObject::ConstIterator specEntry=spec.begin(); specEntry!=spec.end(); specEntry++) {
                        QString specEntryKey = specEntry.key();
                        if(jsonStructObj[specEntryKey].isNull()) { // specs can be overriden
                            jsonStructObj.insert(specEntryKey, specEntry.value());
                            jsonStructObj.remove("param_template");
                            objectChanged = true;
                        }
                    }
                }
                else {
                    errEntry error(ERR_INVALID_PARAM_DEFINITION, key + " : " + linkTo);
                    errList.push_back(error);
                }
            }
            else {
                errEntry error(ERR_INVALID_PARAM_DEFINITION, key + " : null/wrong type");
                errList.push_back(error);
            }
        }
        else if(key == QStringLiteral("param_templates")) {
            continue;
        }
        else if(jsonStructObj[key].isObject() && !jsonStructObj[key].isNull()) {
            QJsonValueRef subValue = jsonStructObj[key];
            QJsonObject subObject = subValue.toObject();
            if(resolveJsonParamTemplatesRecursive(subObject, jsonParamTemplatesObj, errList)) {
                jsonStructObj[key] = subObject;
                objectChanged = true;
            }
        }
    }
    return objectChanged;
}

QSet<QString> cZeraJsonParamsStructure::m_svalidParamEntryKeys = QSet<QString>() << "type" << "min" << "max" << "decimals" << "default" << "list";
QHash<QString, QStringList> cZeraJsonParamsStructure::m_svalidParamTypes {
    { "bool",  QStringList() << "default"},
    { "int",   QStringList() << "min" << "max" << "default"},
    { "float", QStringList() << "min" << "max" << "decimals" << "default"},
    { "string", QStringList() << "default"},
    { "strlist", QStringList() << "list" << "default"},
};


void cZeraJsonParamsStructure::validateParamData(QJsonObject::ConstIterator iter, bool inTemplate, QStringList jsonStructurePathList, cZeraJsonParamsStructure::ErrList &errList)
{
    QJsonValue value = iter.value();
    QString key = iter.key();
    QString treePosPrint = key;
    if(!jsonStructurePathList.isEmpty()) {
        treePosPrint = jsonStructurePathList.join(".") + "." + treePosPrint;
    }
    if(!inTemplate || value.isObject()) {
        QJsonObject paramObject = value.toObject();
        QString type = paramObject["type"].toString();
        bool validType = true;
        if(type.isEmpty()) {
            validType = false;
            if(!inTemplate) { // for templates param type is useful but not mandatory
                errEntry error(ERR_INVALID_PARAM_DEFINITION, treePosPrint + ".type : null");
                errList.push_back(error);
            }
        }
        if(validType) {
            if(!m_svalidParamTypes.contains(type)) {
                validType = false;
                errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                               treePosPrint + ".type" + " : " + type + " invalid");
                errList.push_back(error);
            }
        }
        for(QJsonObject::ConstIterator iterEntries=paramObject.begin(); iterEntries!=paramObject.end(); iterEntries++) {
            // valid data keys?
            QString entryKey = iterEntries.key();
            if(!m_svalidParamEntryKeys.contains(entryKey)) {
                errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                               treePosPrint + "." + entryKey);
                errList.push_back(error);
            }
            // type specific
            if(validType && entryKey != "type") {
                // check if parameter is allowed for type
                QStringList typeParams = m_svalidParamTypes[type]; // safe see checked above
                if(!typeParams.contains(entryKey)) {
                    errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                                   treePosPrint + "." + entryKey + " not allowed for type " + type);
                    errList.push_back(error);
                }
            }
        }
        // all properties are mandatory in resolved structure - otherwise we
        // need to plaster checks all over the place
        if(validType && !inTemplate) {
            QStringList typeParams = m_svalidParamTypes[type];
            for(auto paramRequired : typeParams) {
                if(!paramObject.contains(paramRequired)) {
                    errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                                   treePosPrint + "." + paramRequired + " missing for type " + type);
                    errList.push_back(error);
                }
            }
        }
        // max > min / default out of limit - classic late night bugs introduced
        if(paramObject.contains("max") && paramObject.contains("min")) {
            double min = paramObject["min"].toDouble();
            double max = paramObject["max"].toDouble();
            if(max < min) {
                errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                               treePosPrint + ".max < min");
                errList.push_back(error);
            }
            else if(paramObject.contains("default")) {
                double dbldefault = paramObject["default"].toDouble();
                if(dbldefault < min) {
                    errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                                   treePosPrint + ".default < min");
                    errList.push_back(error);
                }
                if(dbldefault > max) {
                    errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                                   treePosPrint + ".default > max");
                    errList.push_back(error);
                }
            }
        }
        // TODO defaults out of limit / valid data types for other??
    }
    else {
        errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                       value.isNull() ? "null" : key);
        errList.push_back(error);
    }
}

void cZeraJsonParamsStructure::validateResolvedParamDataRecursive(QJsonObject &jsonStructObj, QStringList jsonStructurePathList, cZeraJsonParamsStructure::ErrList &errList)
{
    for(QJsonObject::Iterator sub=jsonStructObj.begin(); sub!=jsonStructObj.end(); sub++) {
        QString key = sub.key();
        if(key == QStringLiteral("params")) {
            if(sub.value().isObject()) {
                QJsonObject subObj = sub.value().toObject();
                for(QJsonObject::ConstIterator iter=subObj.begin(); iter!=subObj.end(); ++iter) {
                    jsonStructurePathList.push_back(key);
                    validateParamData(iter, false, jsonStructurePathList, errList);
                    jsonStructurePathList.pop_back();
                }
            }
        }
        else if(jsonStructObj[key].isObject() && !jsonStructObj[key].isNull()) {
            QJsonValueRef subValue = jsonStructObj[key];
            QJsonObject subObject = subValue.toObject();
            jsonStructurePathList.push_back(key);
            validateResolvedParamDataRecursive(subObject, jsonStructurePathList, errList);
            jsonStructurePathList.pop_back();
        }
    }
}

cZeraJsonParamsStructure::errEntry::errEntry(cZeraJsonParamsStructure::errorTypes errType, QString strInfo) :
    m_errType(errType),
    m_strInfo(strInfo)
{
}
