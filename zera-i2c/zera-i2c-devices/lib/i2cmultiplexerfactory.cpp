#include "i2cmultiplexerfactory.h"
#include <i2cmuxer.h>

I2cMuxerInterface::Ptr I2cMultiplexerFactory::createClampMuxer(QString deviceNode, ushort i2cMuxAdress, quint8 ctrlChannel)
{
    return std::make_shared<I2cMuxer>(deviceNode, i2cMuxAdress, (ctrlChannel-4) | 8, 0);
}

I2cMuxerInterface::Ptr I2cMultiplexerFactory::createNullMuxer()
{
    return std::make_shared<I2cMuxerNull>();
}
