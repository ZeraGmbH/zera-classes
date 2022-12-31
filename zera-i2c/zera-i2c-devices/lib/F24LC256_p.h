#ifndef F24LC256_P_H
#define F24LC256_P_H

#include <QTimer>

#include "i2cutils.h"
#include "i2ceeprom_p.h"

#define Write24LC256 0
#define Read24LC256 1
#define blockReadLen 4096


class cF24LC256Private: public cI2CEEPromPrivate
{
public:
    cF24LC256Private(QString devNode, short adr);
    virtual ~cF24LC256Private(){};
    virtual int WriteData(char* data, ushort count, ushort adr) override;
    virtual int ReadData(char* data, ushort count, ushort adr) override;
    virtual int Reset() override;
    virtual int size() override;
};

#endif // F24LC256_P_H
