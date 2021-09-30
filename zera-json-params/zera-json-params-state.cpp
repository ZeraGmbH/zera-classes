#include "zera-json-params-state.h"
#include <QFile>
#include <QJsonObject>


cZeraJsonParamsState::cZeraJsonParamsState()
{
}

cZeraJsonParamsState::ErrList cZeraJsonParamsState::loadJson(const QByteArray &jsonStructure, const QByteArray &jsonParamState,
                                                   const QString &jsonStructureErrHint, const QString &jsonParamStateErrHint)
{
    ErrList errList;
    // json parse param state
    QJsonDocument jsonDocParamState;
    if(!jsonParamState.isEmpty()) {
        jsonDocParamState = QJsonDocument::fromJson(jsonParamState);
        if(jsonDocParamState.isNull()) {
            errEntry error(ERR_INVALID_JSON, jsonParamStateErrHint.isEmpty() ? jsonParamState : jsonParamStateErrHint);
            errList.push_back(error);
        }
    }
    return errList;
}

cZeraJsonParamsState::ErrList cZeraJsonParamsState::loadJsonFromFiles(const QString &filenameJsonStructure, const QString &filenameJsonParamState)
{
   ErrList errList;
   QFile jsonStructureFile(filenameJsonStructure);
   if(jsonStructureFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
       QByteArray jsonStructure = jsonStructureFile.readAll();
       jsonStructureFile.close();

       QByteArray jsonParams;
       if(!filenameJsonParamState.isEmpty()) {
           QFile jsonParamsFile(filenameJsonParamState);
           if(jsonParamsFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
               jsonParams = jsonParamsFile.readAll();
               jsonParamsFile.close();
           }
           else {
               errEntry error(ERR_FILE_IO, filenameJsonParamState);
               errList.push_back(error);
           }
       }
       if(errList.isEmpty()) {
           errList = loadJson(jsonStructure, jsonParams, filenameJsonStructure, filenameJsonParamState);
       }
   }
   else {
       errEntry error(ERR_FILE_IO, filenameJsonStructure);
       errList.push_back(error);
   }
   return errList;
}

QByteArray cZeraJsonParamsState::exportJson(QJsonDocument::JsonFormat format)
{
    return m_jsonParamState.toJson(format);
}

cZeraJsonParamsState::ErrList cZeraJsonParamsState::exportJsonFile(const QString &filenameJsonParamState, QJsonDocument::JsonFormat format)
{
    ErrList errList;
    QFile jsonJsonParamState(filenameJsonParamState);
    QByteArray jsonData = exportJson(format);
    if(jsonJsonParamState.open(QIODevice::WriteOnly)) {
        jsonJsonParamState.write(jsonData);
        jsonJsonParamState.close();
    }
    else {
        errEntry error(ERR_FILE_IO, filenameJsonParamState);
        errList.push_back(error);
    }
    return errList;
}

cZeraJsonParamsState::ErrList cZeraJsonParamsState::param(const QStringList &paramPath, QVariant& value)
{
    ErrList errList;
    // TODO
    return errList;
}

cZeraJsonParamsState::ErrList cZeraJsonParamsState::setParam(const QStringList &paramPath, QVariant value)
{
    ErrList errList;
    // TODO
    return errList;
}



cZeraJsonParamsState::errEntry::errEntry(cZeraJsonParamsState::errorTypes errType, QString strInfo) :
    m_errType(errType),
    m_strInfo(strInfo)
{
}
