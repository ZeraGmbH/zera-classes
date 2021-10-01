#ifndef CJSONMERGE_H
#define CJSONMERGE_H

#include <QJsonObject>

class cJSONMerge
{
public:
    static void mergeJson(QJsonObject& src, const QJsonObject& other);
};

#endif // CJSONMERGE_H
