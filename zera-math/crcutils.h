#ifndef CRCUTILS_H
#define CRCUTILS_H

#include <QtGlobal>
#include "math_global.h"

/******************************************************************************************/
/**
 * @file crcutils.h
 * @brief crcutils.h holds all necessary declarations implemented crc calculation algorithms
 *
 * @author Peter Lohmer p.lohmer@zera.de
*******************************************************************************************/


class cMaxim1WireCRCPrivate; // forward


/**
  @brief
  cMaxim1WireCRC is a class to support crc generation introduced from MAXIM for 1 wire connections.
  crc is calculated with polynomial = x^8 + x^5 + x^4 + 1
  */
class ZERA_MATHSHARED_EXPORT cMaxim1WireCRC
{
public:
    /**
      @b Initialise the const variables
      */
    cMaxim1WireCRC();
    /**
      @b Calculate the crc
      @param block points to data for which crc shall be calculated
      @param blocklen defines the length of the datafield
      */
    quint8 CalcBlockCRC(quint8* block,quint32 blocklen);
private:
    /**
      @b D'pointer to the private library internal structure

      this is used to hide the internal structure, and thus make the library ABI safe
      */
    cMaxim1WireCRCPrivate* d_ptr;
};

#endif // CRCUTILS_H
