#ifndef I2CMUXERINTERFACE_H
#define I2CMUXERINTERFACE_H

#include <QSharedPointer>

class I2cMuxerInterface
{
public:
    typedef QSharedPointer<I2cMuxerInterface> Ptr;
    virtual void enableMuxChannel() = 0;
    virtual void disableMux() = 0;
};

#endif // I2CMUXERINTERFACE_H
