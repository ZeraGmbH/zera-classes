#include "i2ceeprom_p.h"

cI2CEEPromPrivate::cI2CEEPromPrivate(QString dNode, ushort adr)
    :DevNode(dNode), I2CAdress(adr)
{
}
