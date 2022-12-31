#ifndef CRCUTILS_H
#define CRCUTILS_H

#include "zera-i2c-devices_export.h"
#include <QtGlobal>

/******************************************************************************************/
/**
 * @file crcutils.h
 * @brief crcutils.h holds all necessary declarations implemented crc calculation algorithms
 *
 * @author Peter Lohmer p.lohmer@zera.de
*******************************************************************************************/


/**
  @brief
  cMaxim1WireCRC is a class to support crc generation introduced from MAXIM for 1 wire connections.
  crc is calculated with polynomial = x^8 + x^5 + x^4 + 1
  */
class ZERA_I2C_DEVICES_EXPORT cMaxim1WireCRC
{
public:
    /**
      @b Calculate the crc
      @param block points to data for which crc shall be calculated
      @param blocklen defines the length of the datafield
      @return crc of block
      */
    static quint8 CalcBlockCRC(quint8* block,quint32 blocklen);
    /**
      @b Calculate the crc
      @param crc current checksum
      @param byte to calc next crc for
      @return next crc
      */
    static quint8 CalcByteCRC(quint8 crc, quint8 data);
};

#endif // CRCUTILS_H
