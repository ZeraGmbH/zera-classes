#include "i2cmultiplexerfactory.h"
#include <i2cmuxer.h>
#include <i2cmuxerpca9547.h>

I2cMuxerInterface::Ptr I2cMultiplexerFactory::createClampMuxer(QString deviceNode, ushort i2cMuxAdress, quint8 ctrlChannel)
{
    return std::make_shared<I2cMuxer>(deviceNode, i2cMuxAdress, (ctrlChannel-4) | 8, 0);
}

I2cMuxerInterface::Ptr I2cMultiplexerFactory::createPCA9547Muxer(QString deviceNode, ushort i2cMuxAdress, quint8 channel0to7)
{
    return std::make_shared<I2cMuxerPCA9547>(deviceNode, i2cMuxAdress, channel0to7);
}

I2cMuxerInterface::Ptr I2cMultiplexerFactory::createNullMuxer()
{
    return std::make_shared<I2cMuxerNull>();
}
