#ifndef ZERA_JSON_MERGE_H
#define ZERA_JSON_MERGE_H

#include <QJsonObject>

class cJSONMerge
{
public:
    static void mergeJson(QJsonObject& src, const QJsonObject& other);
};

#endif // ZERA_JSON_MERGE_H
