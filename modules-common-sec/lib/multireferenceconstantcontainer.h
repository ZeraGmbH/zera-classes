#ifndef MULTIREFERENCECONSTANTCONTAINER_H
#define MULTIREFERENCECONSTANTCONTAINER_H

#include "secinputinfo.h"
#include <QHash>
#include <QVariant>

class MultiReferenceConstantContainer
{
public:
    MultiReferenceConstantContainer();
    ~MultiReferenceConstantContainer();
    void addReferenceInput(QString refInputName, QString resource);
    QHash<QString, SecInputInfo*> &getRevResources();
    QString getPowerType(QString refInputName) const;
private:
    QHash<QString /* f0... */, SecInputInfo*> m_refInputInfoHash;
};

#endif // MULTIREFERENCECONSTANTCONTAINER_H
