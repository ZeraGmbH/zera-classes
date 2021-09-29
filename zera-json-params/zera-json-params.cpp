#include "zera-json-params.h"
#include <QFile>
#include <QJsonObject>


cZeraJsonParams::cZeraJsonParams()
{
}

cZeraJsonParams::ErrList cZeraJsonParams::loadJson(const QByteArray &jsonStructure, const QByteArray &jsonParams,
                                                   const QString &jsonStructureErrHint, const QString &jsonParamsErrHint)
{
    ErrList errList;
    QJsonDocument jsonDocStructure = QJsonDocument::fromJson(jsonStructure);
    if(jsonDocStructure.isNull()) {
        errEntry error(ERR_INVALID_JSON, jsonStructureErrHint.isEmpty() ? jsonStructure : jsonStructureErrHint);
        errList.push_back(error);
    }
    QJsonDocument jsonDocParams;
    if(!jsonParams.isEmpty()) {
        jsonDocParams = QJsonDocument::fromJson(jsonParams);
        if(jsonDocParams.isNull()) {
            errEntry error(ERR_INVALID_JSON, jsonParamsErrHint.isEmpty() ? jsonParams : jsonParamsErrHint);
            errList.push_back(error);
        }
    }
    if(errList.isEmpty()) {
        if(jsonDocStructure.isObject()) {
            QJsonObject jsonObjStructure = jsonDocStructure.object();
            resolveJSONParamTemplates(jsonObjStructure, errList);
            // now params are complete for validation
            validateCompletParamDataRecursive(jsonObjStructure, errList);

            m_jsonStructure.setObject(jsonObjStructure);
        }
        else {
            errEntry error(ERR_INVALID_JSON, jsonParamsErrHint.isEmpty() ? jsonParams : jsonParamsErrHint);
            errList.push_back(error);
        }
    }
    return errList;
}

cZeraJsonParams::ErrList cZeraJsonParams::loadJsonFromFiles(const QString &filenameJSONStructure, const QString &filenameJSONParams)
{
   ErrList errList;
   QFile jsonStructureFile(filenameJSONStructure);
   if(jsonStructureFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
       QByteArray jsonStructure = jsonStructureFile.readAll();
       jsonStructureFile.close();

       QByteArray jsonParams;
       if(!filenameJSONParams.isEmpty()) {
           QFile jsonParamsFile(filenameJSONParams);
           if(jsonParamsFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
               jsonParams = jsonParamsFile.readAll();
               jsonParamsFile.close();
           }
           else {
               errEntry error(ERR_FILE_IO, filenameJSONParams);
               errList.push_back(error);
           }
       }
       if(errList.isEmpty()) {
           errList = loadJson(jsonStructure, jsonParams, filenameJSONStructure, filenameJSONParams);
       }
   }
   else {
       errEntry error(ERR_FILE_IO, filenameJSONStructure);
       errList.push_back(error);
   }
   return errList;
}

QByteArray cZeraJsonParams::exportJson()
{
    return m_jsonParams.toJson();
}

cZeraJsonParams::ErrList cZeraJsonParams::exportJsonFile(const QString &filenameJSON)
{
    ErrList errList;
    QFile jsonStructureFile(filenameJSON);
    QByteArray jsonData = exportJson();
    if(jsonStructureFile.open(QIODevice::WriteOnly)) {
        jsonStructureFile.write(jsonData);
        jsonStructureFile.close();
    }
    else {
        errEntry error(ERR_FILE_IO, filenameJSON);
        errList.push_back(error);
    }
    return errList;
}

cZeraJsonParams::ErrList cZeraJsonParams::param(const QStringList &paramPath, QVariant& val)
{
    ErrList errList;
    // TODO
    return errList;
}

cZeraJsonParams::ErrList cZeraJsonParams::setParam(const QStringList &paramPath, QVariant value)
{
    ErrList errList;
    // TODO
    return errList;
}

void cZeraJsonParams::resolveJSONParamTemplates(QJsonObject &jsonStructObj, ErrList& errList)
{
    // Find "param_templates" object and start recursive resolve
    QJsonValue paramTemplateValue = jsonStructObj["param_templates"];
    if(!paramTemplateValue.isNull()) { // param_templates can be ommitted
        if(paramTemplateValue.isObject()) {
            QJsonObject jsonParamTemplatesObj = paramTemplateValue.toObject();
            // validate param_templates
            for(QJsonObject::ConstIterator iter=jsonParamTemplatesObj.begin(); iter!=jsonParamTemplatesObj.end(); ++iter) {
                validateParamData(iter, true, errList);
            }
            resolveJSONParamTemplatesRecursive(jsonStructObj, jsonParamTemplatesObj, errList);
        }
        else {
            errEntry error(ERR_INVALID_PARAM_TEMPLATE, QStringLiteral("param_templates"));
            errList.push_back(error);
        }
    }
}

bool cZeraJsonParams::resolveJSONParamTemplatesRecursive(QJsonObject& jsonStructObj, const QJsonObject jsonParamTemplatesObj, ErrList& errList)
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
            if(resolveJSONParamTemplatesRecursive(subObject, jsonParamTemplatesObj, errList)) {
                jsonStructObj[key] = subObject;
                objectChanged = true;
            }
        }
    }
    return objectChanged;
}

QSet<QString> cZeraJsonParams::m_svalidParamEntryKeys = QSet<QString>() << "type" << "min" << "max" << "decimals" << "default" << "list";
QHash<QString, QStringList> cZeraJsonParams::m_svalidParamTypes {
    { "bool",  QStringList() << "default"},
    { "int",   QStringList() << "min" << "max" << "default"},
    { "float", QStringList() << "min" << "max" << "decimals" << "default"},
    { "string", QStringList() << "default"},
    { "strlist", QStringList() << "list" << "default"},
};


void cZeraJsonParams::validateParamData(QJsonObject::ConstIterator iter, bool inTemplate, cZeraJsonParams::ErrList &errList)
{
    QJsonValue value = iter.value();
    QString key = iter.key();
    if(!inTemplate || value.isObject()) {
        QJsonObject paramObject = value.toObject();
        QString type = paramObject["type"].toString();
        bool validType = true;
        if(type.isEmpty()) {
            validType = false;
            if(!inTemplate) { // for templates param type is useful but not mandatory
                errEntry error(ERR_INVALID_PARAM_DEFINITION, key + ".type : null");
                errList.push_back(error);
            }
        }
        if(validType) {
            if(!m_svalidParamTypes.contains(type)) {
                validType = false;
                errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                               key + ".type" + " : " + type + " invalid");
                errList.push_back(error);
            }
        }
        for(QJsonObject::ConstIterator iterEntries=paramObject.begin(); iterEntries!=paramObject.end(); iterEntries++) {
            // valid data keys?
            QString entryKey = iterEntries.key();
            if(!m_svalidParamEntryKeys.contains(entryKey) &&
                    (!inTemplate && entryKey != "param_template")) {
                errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                               key + "." + entryKey);
                errList.push_back(error);
            }
            // type specific
            if(validType && entryKey != "type") {
                QStringList typeParams = m_svalidParamTypes[type]; // safe see checked above
                if(!typeParams.contains(entryKey) && entryKey != "param_template") {
                    errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                                   key + "." + entryKey + " not allowed for type " + type);
                    errList.push_back(error);
                }
            }
        }
        // max > min - a classic late night bug introduced
        if(paramObject.contains("max") && paramObject.contains("min")) {
            if(paramObject["max"].toDouble() < paramObject["min"].toDouble()) {
                errEntry error(inTemplate ? ERR_INVALID_PARAM_TEMPLATE_DEFINITION : ERR_INVALID_PARAM_DEFINITION,
                               key + " max < min");
                errList.push_back(error);
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

void cZeraJsonParams::validateCompletParamDataRecursive(QJsonObject &jsonStructObj, cZeraJsonParams::ErrList &errList)
{
    for(QJsonObject::Iterator sub=jsonStructObj.begin(); sub!=jsonStructObj.end(); sub++) {
        QString key = sub.key();
        if(key == QStringLiteral("params")) {
            if(sub.value().isObject()) {
                QJsonObject subObj = sub.value().toObject();
                for(QJsonObject::ConstIterator iter=subObj.begin(); iter!=subObj.end(); ++iter) {
                    validateParamData(iter, false, errList);
                }
            }
        }
        // param_templates were validated already on resolveJSONParamTemplates
        else if(key == QStringLiteral("param_templates")) {
            continue;
        }
        else if(jsonStructObj[key].isObject() && !jsonStructObj[key].isNull()) {
            QJsonValueRef subValue = jsonStructObj[key];
            QJsonObject subObject = subValue.toObject();
            validateCompletParamDataRecursive(subObject, errList);
        }
    }
}

cZeraJsonParams::errEntry::errEntry(cZeraJsonParams::errorTypes errType, QString strInfo) :
    m_errType(errType),
    m_strInfo(strInfo)
{
}
