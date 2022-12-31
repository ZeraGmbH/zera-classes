#ifndef F24LC256_H
#define F24LC256_H

#include "zera-i2c-devices_export.h"
#include "i2ceeprom_p.h"
#include <QtCore/qglobal.h>

class cF24LC256Private;

class ZERA_I2C_DEVICES_EXPORT cF24LC256: public cI2CEEPromPrivate
{
public:
    cF24LC256(QString devNode, short adr);
    virtual ~cF24LC256();
    virtual int WriteData(char* data, ushort count, ushort adr);
    virtual int ReadData(char* data, ushort count, ushort adr);
    virtual int Reset();
    virtual int size();
private:
    cF24LC256Private* d_ptr;
};

#endif // F24LC256_H
