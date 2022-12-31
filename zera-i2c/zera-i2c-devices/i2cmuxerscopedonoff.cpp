#include "i2cmuxerscopedonoff.h"
#include "singleonoffreferencemanager.h"

SingleOnOffReferenceManager<QString> I2cMuxerScopedOnOff::m_onOffManag;

I2cMuxerScopedOnOff::I2cMuxerScopedOnOff(I2cMuxerInterface::Ptr i2cMuxer) :
    m_i2cMuxer(i2cMuxer)
{
    QString i2cId = m_i2cMuxer->getDevIdString();
    m_onOffManag.addRef(i2cId);
    if(m_onOffManag.isFirstRef(i2cId)) {
        m_i2cMuxer->enableMuxChannel();
    }
}

I2cMuxerScopedOnOff::~I2cMuxerScopedOnOff()
{
    QString i2cId = m_i2cMuxer->getDevIdString();
    m_onOffManag.freeRef(i2cId);
    if(m_onOffManag.hasNoRefs(i2cId)) {
        m_i2cMuxer->disableMux();
    }
}
