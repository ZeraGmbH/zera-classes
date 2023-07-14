#include "multireferenceconstantcontainer.h"

MultiReferenceConstantContainer::MultiReferenceConstantContainer()
{
}

MultiReferenceConstantContainer::~MultiReferenceConstantContainer()
{
    for(auto refInputInfo : qAsConst(m_refInputInfoHash)) {
        delete refInputInfo;
    }
}

void MultiReferenceConstantContainer::addReferenceInput(QString refInputName, QString resource)
{
    m_refInputInfoHash[refInputName] = new SecInputInfo();
    m_refInputInfoHash[refInputName]->name = refInputName;
    m_refInputInfoHash[refInputName]->resource = resource;
}

QHash<QString, SecInputInfo *> &MultiReferenceConstantContainer::getRevResources()
{
    return m_refInputInfoHash;
}

QString MultiReferenceConstantContainer::getPowerType(QString refInputName) const
{
    return m_refInputInfoHash[refInputName]->alias;
}
