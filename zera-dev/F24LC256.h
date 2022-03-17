#ifndef F24LC256_H
#define F24LC256_H

#include "dev_global.h"
#include "i2ceeprom_p.h"

class cF24LC256Private;

class ZERADEV_EXPORT cF24LC256: public cI2CEEPromPrivate
{
public:
    cF24LC256(QString devNode,int dLevel,short adr);
    virtual ~cF24LC256();
    virtual int WriteData(char* data, ushort count, ushort adr);
    virtual int ReadData(char* data, ushort count, ushort adr);
    virtual int Reset();
    virtual int size();
private:
    cF24LC256Private* d_ptr;
};

#endif // F24LC256_H
