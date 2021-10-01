#include "jsonexport.h"

QByteArray cJsonExport::exportJson(const QJsonObject& jsonObj, QJsonDocument::JsonFormat format)
{
    QByteArray jsonData = "{}";
    if(!jsonObj.isEmpty()) {
        QJsonDocument jsonDoc;
        jsonDoc.setObject(jsonObj);
        jsonData = jsonDoc.toJson(format);
    }
    return jsonData;
}
