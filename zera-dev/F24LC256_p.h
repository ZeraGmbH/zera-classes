#ifndef F24LC256_P_H
#define F24LC256_P_H

#include "i2cutils.h"
#include "i2ceeprom_p.h"

#define Write24LC256 0
#define Read24LC256 1

class cF24LC256Private: public cI2CEEPromPrivate
{
public:
    cF24LC256Private(QString devNode,int dLevel,short adr);
    virtual ~cF24LC256Private(){};
    virtual int WriteData(char* data, ushort count, ushort adr);
    virtual int ReadData(char* data, ushort count, ushort adr);
    virtual int size();
};

#endif // F24LC256_P_H
