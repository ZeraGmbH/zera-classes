#include "zera-json-merge.h"
#include <QJsonArray>

void cJSONMerge::mergeJson(QJsonObject &src, const QJsonObject& other)
{
    // taken from https://stackoverflow.com/questions/64665582/how-to-merge-two-qjsonobjects-in-qt
    for(auto it = other.constBegin(); it != other.constEnd(); ++it) {
        if(src.contains(it.key())) {
            if(src.value(it.key()).isObject() && other.value(it.key()).isObject()) {
                QJsonObject one(src.value(it.key()).toObject());
                QJsonObject two(other.value(it.key()).toObject());

                mergeJson(one, two);
                src[it.key()] = one;
            }
            else if(src.value(it.key()).isArray() && other.value(it.key()).isArray()) {
                QJsonArray arr = other.value(it.key()).toArray();
                QJsonArray srcArr = src.value(it.key()).toArray();
                for(int i = 0; i < arr.size(); i++)
                    srcArr.append(arr[i]);
                src[it.key()] = srcArr;
            }
        }
        else {
            src[it.key()] = it.value();
        }
    }
}
