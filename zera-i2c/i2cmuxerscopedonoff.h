#ifndef I2CMUXERSCOPEDONOFF_H
#define I2CMUXERSCOPEDONOFF_H

#include "i2cmuxerinterface.h"

class I2cMuxerScopedOnOff
{
public:
    I2cMuxerScopedOnOff(I2cMuxerInterface::Ptr i2cMuxer);
    virtual ~I2cMuxerScopedOnOff();
private:
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // I2CMUXERSCOPEDONOFF_H
