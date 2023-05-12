#ifndef I2CMUXERINTERFACE_H
#define I2CMUXERINTERFACE_H

#include <QString>
#include <memory>

class I2cMuxerInterface
{
public:
    typedef std::shared_ptr<I2cMuxerInterface> Ptr;
    virtual void enableMuxChannel() = 0;
    virtual void disableMux() = 0;
    virtual QString getDevIdString() = 0;
};

#endif // I2CMUXERINTERFACE_H
