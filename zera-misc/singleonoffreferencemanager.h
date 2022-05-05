#ifndef SINGLEONOFFREFERENCEMANAGER_H
#define SINGLEONOFFREFERENCEMANAGER_H

#include <QMap>
template <class T>
class SingleOnOffReferenceManager
{
public:
    void addRef(T key) {
        auto iter = referenceCount.find(key);
        if(iter == referenceCount.end()) {
            referenceCount[key] = 1;
        }
        else {
            iter.value()++;
        }
    }
    void freeRef(T key) {
        auto iter = referenceCount.find(key);
        if(iter != referenceCount.end() && iter.value() > 0) {
            iter.value()--;
        }
    }
    bool isFirstRef(T key) {
        auto iter = referenceCount.find(key);
        return iter != referenceCount.end() && iter.value() == 1;
    }
    bool hasNoRefs(T key) {
        auto iter = referenceCount.find(key);
        return iter == referenceCount.end() || iter.value() == 0;
    }
private:
    QMap<T, int> referenceCount;
};

#endif // SINGLEONOFFREFERENCEMANAGER_H
