#ifndef ZERA_JSON_EXPORT_H
#define ZERA_JSON_EXPORT_H

#include <QJsonObject>
#include <QJsonDocument>

class cJsonExport
{
public:
    static QByteArray exportJson(const QJsonObject& jsonObj, QJsonDocument::JsonFormat format);
};

#endif // ZERA_JSON_EXPORT_H
