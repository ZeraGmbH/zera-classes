#ifndef CRCUTILS_P_H
#define CRCUTILS_P_H

#include <QtGlobal>

/**
  @brief
  The implemention for cMaxim1WireCRC.
  */

class cMaxim1WireCRCPrivate
{
public:
    /**
      @b Initialise the const variables
      */
    cMaxim1WireCRCPrivate();
    /**
      @b Calculate the crc
      @param block points to data for which crc shall be calculated
      @param blocklen defines the length of the datafield
      */
    quint8 CalcBlockCRC(quint8*,quint32);
private:
    /**
      @b Calculate the crc
      @param block points to data for which crc shall be calculated
      @param blocklen defines the length of the datafield
      */
    quint8 CalcByteCRC(quint8,quint8);
};

#endif // CRCUTILS_P_H
