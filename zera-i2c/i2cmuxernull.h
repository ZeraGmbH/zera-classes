#ifndef I2CMUXERNULL_H
#define I2CMUXERNULL_H

#include "i2cmuxerinterface.h"

class I2cMuxerNull : public I2cMuxerInterface
{
public:
    virtual void enableMuxChannel() {}
    virtual void disableMux() {}
};

#endif // I2CMUXERNULL_H
