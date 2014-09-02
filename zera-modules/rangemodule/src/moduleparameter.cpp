#include "moduleparameter.h"
#include "veinpeer.h"
#include "veinentity.h"

namespace RANGEMODULE
{

cModuleParameter::cModuleParameter(VeinPeer *peer, QString name, QVariant initval, bool readonly)
    :m_pPeer(peer)
{
    m_pParEntity = m_pPeer->dataAdd(name);
    m_pParEntity->setValue(initval, peer);
    m_pParEntity->modifiersAdd(VeinEntity::MOD_NOECHO);
    if (readonly)
        m_pParEntity->modifiersAdd(VeinEntity::MOD_READONLY);

    connect(m_pParEntity, SIGNAL(sigValueChanged(QVariant)), this, SIGNAL(updated(QVariant)));
}


cModuleParameter::~cModuleParameter()
{
    m_pPeer->dataRemove(m_pParEntity);
}


void cModuleParameter::setData(QVariant val)
{
    m_pParEntity->setValue(val, m_pPeer);
}


QVariant cModuleParameter::getData()
{
    return m_pParEntity->getValue();
}

}





