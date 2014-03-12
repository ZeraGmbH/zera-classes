#include "moduleparameter.h"
#include "veinpeer.h"
#include "veinentity.h"

cModuleParameter::cModuleParameter(VeinPeer *peer, QString name, QVariant initval, QString limitname, QVariant limitval)
    :m_pPeer(peer)
{
    m_pParEntity = m_pPeer->dataAdd(name);
    m_pParEntity->setRealValue(initval, peer);
    m_pParEntity->modifiersAdd(VeinEntity::MOD_NOECHO);

    m_pLimitEntity = m_pPeer->dataAdd(limitname);
    m_pLimitEntity->setRealValue(limitval, m_pPeer);
    m_pLimitEntity->modifiersAdd(VeinEntity::MOD_CONST);

    connect(m_pParEntity, SIGNAL(updated()), this, SIGNAL(updated()));
}


cModuleParameter::~cModuleParameter()
{
    m_pPeer->dataRemove(m_pParEntity);
    m_pPeer->dataRemove(m_pLimitEntity);
}


void cModuleParameter::setData(QVariant val)
{
    m_pParEntity->setRealValue(val, m_pPeer);
}


QVariant cModuleParameter::getData()
{
    return m_pParEntity->getValue();
}







