#include <veinpeer.h>
#include <veinentity.h>

#include "modulesignal.h"

namespace THDNMODULE
{

cModuleSignal::cModuleSignal(VeinPeer *peer, QString name, QVariant initval)
    :m_pPeer(peer)
{
    m_pParEntity = m_pPeer->dataAdd(name);
    m_pParEntity->setValue(initval, m_pPeer);
    m_pParEntity->modifiersAdd(VeinEntity::MOD_READONLY);
}


cModuleSignal::~cModuleSignal()
{
    m_pPeer->dataRemove(m_pParEntity);
}

}
