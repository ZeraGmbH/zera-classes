#include <QVariant>

#include "veinpeer.h"
#include "veinentity.h"
#include "moduleerror.h"


cModuleError::cModuleError(VeinPeer *peer, QString name)
    :m_pPeer(peer)
{
    m_sMessage ="";
    m_pParEntity = m_pPeer->dataAdd(name);
    m_pParEntity->setValue(QVariant(m_sMessage), peer);
    m_pParEntity->modifiersAdd(VeinEntity::MOD_NOECHO);

}


void cModuleError::appendMsg(QString msg)
{
    m_sMessage += (msg +";");
    m_pParEntity->setValue(QVariant(m_sMessage), m_pPeer);
}

