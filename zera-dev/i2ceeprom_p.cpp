#include "i2ceeprom_p.h"

cI2CEEPromPrivate::cI2CEEPromPrivate(QString dNode, int dLevel, ushort adr)
    :DevNode(dNode), DebugLevel(dLevel), I2CAdress(adr)
{
}

void cI2CEEPromPrivate::EEPromTimeout()
{
    m_bEEpromTimeout = true;
}
