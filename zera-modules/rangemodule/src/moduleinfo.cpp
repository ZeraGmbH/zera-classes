#include <veinpeer.h>
#include <veinentity.h>

#include "moduleinfo.h"

namespace RANGEMODULE
{

cModuleInfo::cModuleInfo(VeinPeer *peer, QString name, QVariant initval)
    :m_pPeer(peer)
{
    m_pParEntity = m_pPeer->dataAdd(name);
    m_pParEntity->setValue(initval, m_pPeer);
    m_pParEntity->modifiersAdd(VeinEntity::MOD_CONST);
}

cModuleInfo::~cModuleInfo()
{
    m_pPeer->dataRemove(m_pParEntity);
}

}


