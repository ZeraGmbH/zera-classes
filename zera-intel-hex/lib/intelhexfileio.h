#ifndef INTELHEXFILEIO_H
#define INTELHEXFILEIO_H

#include "zera-intel-hex_export.h"
#include <QString>
#include <QList>

class cIntelHexFileIOPrivate; // forward

/**
  @brief
  cIntelHexFileIO is a class to support intel hex file io management.

  It reads intel hex format from a given file and reports detected errors.
  While reading the file it fills a list with all memory regions found.
  Afterwards user can request memory regions to fill.
  orig. written for mfc, later ported to qt4 by Peter Lohmer (p.lohmer@zera.de)
  @author Andreas Müller  a.mueller@zera.de
  */


class ZERA_INTEL_HEX_EXPORT cIntelHexFileIO
{
public:
    cIntelHexFileIO();
    virtual ~cIntelHexFileIO();

    /**
      @b Read the entire file and parse for memory regions.
      @param fileName is the files full path name.
      */
    bool ReadHexFile(const QString& fileName);
    /**
      @b Set byte value which is used to set unwritten regions.
      */
    void setunwrittenByteValue(quint8 value);
    QString getErrorsWarnings();

    /**
      Memory Read iterated: In case more than one block meet address range: Stuff empty regions between
      with unwritten byte value. End is signalled by an empty array. If no block is found for the adress,
      he address is incremented to the next block found in steps of dwBlockLen to ensure to write one
      flash block only once. By adding dwStartAddressModuloBlockLen+dwOffsetToModulo the effective
      start address of the memory block in byteArray is calculated.
      */
    bool GetMemoryBlock(const quint32& nBlockLen, quint32& nStartAddressModuloBlockLen, QByteArray& byteArray, quint32& nOffsetToModulo);
    quint32 getByteCountStuffed();
    /**
     * @brief isEmpty
     * @return true if no memory reagions were read
     */
    bool isEmpty();
    bool contains(const QByteArray& byteArray);

private:
    cIntelHexFileIOPrivate* d_ptr;

};


#endif // INTELHEXFILEIO_H
