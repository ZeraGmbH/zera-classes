#include "F24LC256.h"
#include "F24LC256_p.h"

cF24LC256::cF24LC256(QString devNode, int dLevel, short adr)
    :d_ptr( new cF24LC256Private(devNode, dLevel, adr))
{
}

int cF24LC256::WriteData(char *data, ushort count, ushort adr)
{
    return d_ptr->WriteData(data, count, adr);
}

int cF24LC256::ReadData(char *data, ushort count, ushort adr)
{
    return d_ptr->ReadData(data, count, adr);
}

int cF24LC256::size()
{
    return d_ptr->size();
}




