#include "recorderjsonexportveingethandler.h"
#include <QJsonObject>
#include <QJsonDocument>

RecorderJsonExportVeinGetHandler::RecorderJsonExportVeinGetHandler(VeinStorage::StorageRecordDataPtr storedData) :
    m_storedData(storedData)
{
}

const QVariant &RecorderJsonExportVeinGetHandler::getCustomizedValue(const QVariant &currentValue)
{
    Q_UNUSED(currentValue)
    QJsonObject json;


    QJsonDocument doc(json);
    m_jsonExport = doc.toJson(QJsonDocument::Indented);
    return m_jsonExport;
}
