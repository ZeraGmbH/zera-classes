#include <QVariant>

#include "veinpeer.h"
#include "veinentity.h"
#include "moduleerror.h"

namespace RANGEMODULE
{

cModuleError::cModuleError(VeinPeer *peer, QString name)
    :m_pPeer(peer)
{
    m_sMessage ="";
    m_pParEntity = m_pPeer->dataAdd(name);
    m_pParEntity->setValue(QVariant(m_sMessage), peer);
    m_pParEntity->modifiersAdd(VeinEntity::MOD_NOECHO);

    connect(m_pParEntity, SIGNAL(sigValueChanged(QVariant)), this, SLOT(update(QVariant)));
}


void cModuleError::appendMsg(QString msg)
{
    m_sMessage += (msg +";");
    m_pParEntity->setValue(QVariant(m_sMessage), m_pPeer);
}

void cModuleError::update(QVariant)
{
    m_sMessage = "";
    m_pParEntity->setValue(QVariant(m_sMessage), m_pPeer);
}

}
