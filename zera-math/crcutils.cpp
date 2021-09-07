#include "crcutils.h"

quint8 cMaxim1WireCRC::CalcBlockCRC(quint8* pBuff, quint32 iBuffLen)
{
    quint8 crc = 0;
    for (quint32 i = 0; i < iBuffLen; i++)
        crc = CalcByteCRC(crc, pBuff[i]);
    return crc;
}

quint8 cMaxim1WireCRC::CalcByteCRC(quint8 crc, quint8 data)
{
    crc = crc ^ data;
    for (quint8 i = 0; i < 8; i++)
    {
        if (crc & 0x01)
            crc = (crc >> 1) ^ 0x8c;
        else
            crc >>= 1;
    }
    return crc;
}
