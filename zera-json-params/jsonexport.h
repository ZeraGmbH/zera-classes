#ifndef CJSONEXPORT_H
#define CJSONEXPORT_H

#include <QJsonObject>
#include <QJsonDocument>

class cJsonExport
{
public:
    static QByteArray exportJson(const QJsonObject& jsonObj, QJsonDocument::JsonFormat format);
};

#endif // CJSONEXPORT_H
