#include "moduleparameter.h"
#include "paramvalidator.h"
#include "veinpeer.h"
#include "veinentity.h"
#include <vcmp_componentdata.h>


cModuleParameter::cModuleParameter(VeinPeer *peer, QString name, QVariant initval, bool readonly, cParamValidator *pValidator)
    :m_pPeer(peer), m_sName(name), m_pParamValidator(pValidator)
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


QString cModuleParameter::getName()
{
    return m_sName;
}


bool cModuleParameter::paramTransaction(VeinComponent::ComponentData *cdata)
{
    bool retval = false;
    if (cdata->isValid() && (cdata->componentName() == m_sName))
    {
        if (m_pParamValidator)
        {
            if (m_pParamValidator->isValidParam(cdata->newValue()) )
            {
                // newValue is a valid parameter...so we emit a signal for setting
            }

        }
        else

    }

    return retval;

}





