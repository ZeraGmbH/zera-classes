#ifndef I2CMULTIPLEXERFACTORY_H
#define I2CMULTIPLEXERFACTORY_H

#include "i2cmuxernull.h"

class I2cMultiplexerFactory
{
public:
    static I2cMuxerInterface::Ptr createClampMuxer(QString deviceNode, ushort i2cMuxAdress, quint8 ctrlChannel);
    static I2cMuxerInterface::Ptr createPCA9547Muxer(QString deviceNode, ushort i2cMuxAdress, quint8 channel0to7);
    static I2cMuxerInterface::Ptr createNullMuxer();
};

#endif // I2CMULTIPLEXERFACTORY_H
