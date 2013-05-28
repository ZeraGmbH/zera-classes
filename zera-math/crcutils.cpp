#include "crcutils.h"
#include "crcutils_p.h"

cMaxim1WireCRC::cMaxim1WireCRC()
    :d_ptr( new cMaxim1WireCRCPrivate() )
{
}


quint8 cMaxim1WireCRC::CalcBlockCRC(quint8* pBuff, quint32 iBuffLen)
{
    return d_ptr->CalcBlockCRC( pBuff, iBuffLen);
}
