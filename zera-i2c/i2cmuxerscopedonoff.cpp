#include "i2cmuxerscopedonoff.h"

I2cMuxerScopedOnOff::I2cMuxerScopedOnOff(I2cMuxerInterface::Ptr i2cMuxer) :
    m_i2cMuxer(i2cMuxer)
{
    m_i2cMuxer->enableMuxChannel();
}

I2cMuxerScopedOnOff::~I2cMuxerScopedOnOff()
{
    m_i2cMuxer->disableMux();
}
