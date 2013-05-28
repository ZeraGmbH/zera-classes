#ifndef INTELHEXFILEIO_P_H
#define INTELHEXFILEIO_P_H

#include <QString>
#include <QList>

#include "zera-misc_global.h"

struct THexFileMemRegion
{
    // data
    quint32 nStartAddress;
    QByteArray ByteArrContent;
    // methods
    THexFileMemRegion();
    THexFileMemRegion( int StartAddress, const QByteArray& byteArray);
    THexFileMemRegion& operator = (const THexFileMemRegion& obj);
    bool operator == (const THexFileMemRegion& obj) const;
    quint32 GetMaxAddress();
};


struct THexFileMessage
{
    // data
    ulong nLineNo;
    QString sMsgText;				// either string
    // methods
    THexFileMessage(){};
    THexFileMessage(ulong _nLineNo, const QString& _sMsgText);
};


/**
  @brief
  The implemention for cIntelHexFileIO.
  */


class cIntelHexFileIOPrivate
{
public:
    /**
      @b Initialise the const variables
      */
    cIntelHexFileIOPrivate();
    virtual ~cIntelHexFileIOPrivate();

    /**
      @b Read the entire file and parse for memory regions.
      @param fileName is the files full path name.
      */
    bool ReadHexFile(const QString& fileName);
    /**
      @b Set byte value which is used to set unwritten regions.
      */
    void setunwrittenByteValue(quint8 value);
    /**
      @b Return errors and warnings that occured while reading.
      */
    QString getErrorsWarnings();

    /**
      Memory Read iterated: In case more than one block meet address range: Stuff empty regions between
      with unwritten byte value. End is signalled by an empty array. If no block is found for the adress,
      he address is incremented to the next block found in steps of dwBlockLen to ensure to write one
      flash block only once. By adding dwStartAddressModuloBlockLen+dwOffsetToModulo the effective
      start address of the memory block in byteArray is calculated.
      */
    void GetMemoryBlock(const quint32& nBlockLen, quint32& nStartAddressModuloBlockLen, QByteArray& byteArray, quint32& nOffsetToModulo);


private:
    bool isHexText(const char c);
    uchar Hex2Bin(const char* c);
    uchar ASCII2Hex(char c);
    QList<THexFileMemRegion> m_ListMemRegions;
    QList<THexFileMessage> m_sListErrWarn;
    QString m_strFileName;
    uchar m_byteErasedByteValue;
};


#endif // INTELHEXFILEIO_P_H
